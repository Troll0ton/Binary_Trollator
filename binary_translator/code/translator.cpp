#include "binary_translator/include/translator.h" 

//-----------------------------------------------------------------------------

#define CURR_IR_NODE ir->buffer[i]

X64_code *translateIrToX64 (IR *ir, int bin_size)
{
    printf ("-- write commands\n\n");

    X64_code *x64_code = x64CodeCtor (X64_CODE_INIT_SIZE, PAGE_SIZE);
    CodeX64DumpHeader (x64_code);

    // so we can store only RAM_INIT_SIZE / 8 nums in ram
    Ram *ram = ramCtor (RAM_INIT_SIZE, PAGE_SIZE);

    Jmp_table *jmp_table = jmpTableCtor (ir->size);

    saveDataAddress (x64_code, ram->buffer); // save absolute address of RAM in R12 

    #ifndef ELF_MODE
    writeMaskingOp (OP_POP_REG, MASK_R10); // save return address
    #endif
    
    // now translate IR to opcodes and in filling jmp table
    for(int i = 0; i < ir->size; i++)
    {
        jmp_table->buffer[i] = CURR_POS; // fill jump table
        CURR_IR_NODE.x64_pos = CURR_POS;

        translateCmd (x64_code, &CURR_IR_NODE); // translate command
    }
    
    jmpTableDump (jmp_table);

    // fill jmp targets in x64_code (from jmp table) 
    char *in_addr  = (char*) NULL;
    char *out_addr = (char*) FUNCT_ADDR;
    handleJmpTargetsX64 (x64_code, ir, jmp_table, in_addr, out_addr); 

    jmpTableDtor (jmp_table);
    ramDtor (ram);

    printf ("Bin translation complete!\n\n");

    return x64_code;
}

//-----------------------------------------------------------------------------

X64_code *x64CodeCtor (int init_size, int alignment)
{
    X64_code *x64_code  = (X64_code*) calloc        (1,        sizeof (X64_code));
    x64_code->buffer    = (char*)     alignedCalloc (PAGE_SIZE, init_size); 
    x64_code->curr_pos  = x64_code->buffer;

    // Because of unknowing of the final size, the buffer is self-expanding 
    x64_code->capacity  = init_size; 
    x64_code->dump_file = fopen ("binary_translator/dump/x64_dump.txt", "w+");
    // ??

    return x64_code;
}

//-----------------------------------------------------------------------------

void x64CodeResize (X64_code *x64_code)
{
    // There is no analogue of realloc for aligned buffers, so I simply copy old buffer into the new
    char *new_buffer = (char*) alignedCalloc (PAGE_SIZE, 
                                              x64_code->capacity + X64_CODE_INCREASE_PAR); 

    memcpy (new_buffer, x64_code->buffer, x64_code->capacity);
    free   (x64_code->buffer);

    x64_code->buffer = new_buffer;

    // new_size = old_size + X64_CODE_INCREASE_PAR
    x64_code->capacity += X64_CODE_INCREASE_PAR;
}

//-----------------------------------------------------------------------------

void x64CodeDtor (X64_code *x64_code)
{
    x64_code->capacity = DELETED;
    x64_code->size     = DELETED;
    x64_code->curr_pos = NULL;

    free (x64_code->buffer);
    free (x64_code);
}

//-----------------------------------------------------------------------------

Ram *ramCtor (int size, int alignment)
{
    Ram *ram    = (Ram*)  calloc        (1,        sizeof (Ram));
    ram->buffer = (char*) alignedCalloc (PAGE_SIZE, size);
    ram->size   = size;

    return ram;
}

//-----------------------------------------------------------------------------

void ramDtor (Ram *ram)
{
    ram->size = DELETED;

    free (ram->buffer);
    free (ram);
}
 
//-----------------------------------------------------------------------------

Jmp_table *jmpTableCtor (int size)
{
    // this version of jump table contains pointers to all instructions,
    // which are represented in IR buffer

    Jmp_table *jmp_table = (Jmp_table*) calloc (1,    sizeof (Jmp_table));
    jmp_table->buffer    = (char**)     calloc (size, sizeof (char*));
    jmp_table->size      = size;
    jmp_table->dump_file = fopen ("binary_translator/dump/jump_table.txt", "w+");

    return jmp_table;
}

//-----------------------------------------------------------------------------

void jmpTableDtor (Jmp_table *jmp_table)
{
    jmp_table->size = DELETED;

    free   (jmp_table->buffer);
    free   (jmp_table);
    fclose (jmp_table->dump_file);
}

//-----------------------------------------------------------------------------

void writePrologue (X64_code *x64_code)
{
    // create lite version of stack's frame
    writeSimpleOp  (OP_PUSHA);
    writeMaskingOp (OP_PUSH_REG, MASK_RBP);
    writeSimpleOp  (OP_MOV_RBP_RSP);

    writeSimpleOp  (OP_ALIGN_STK); 
    writeMaskingOp (OP_SUB_REG_IMM, MASK_RSP);
    writeInt32     (8);
    writeMaskingOp (OP_PUSH_REG, MASK_RBP);
}

//-----------------------------------------------------------------------------

void writeEpilogue (X64_code *x64_code) 
{
    writeMaskingOp (OP_POP_REG, MASK_RBP);
    
    writeSimpleOp  (OP_MOV_RSP_RBP);
    writeMaskingOp (OP_POP_REG, MASK_RBP);
    writeSimpleOp  (OP_POPA);
}

//-----------------------------------------------------------------------------

void saveDataAddress (X64_code *x64_code, char *ram) // r12 <- ptr to 'ram' (troll-code representation)
{
    writeMaskingOp (OP_MOV_REG_IMM, MASK_R12);

    #ifdef ELF_MODE
    uint64_t abs_ptr = (uint64_t)(x64_code->buffer + RAM_ADDR - TEXT_ADDR); // make correct addressing 
    #else
    uint64_t abs_ptr = (uint64_t)(ram); 
    #endif

    writeAbsPtr (abs_ptr);
}

//-----------------------------------------------------------------------------

void *alignedCalloc (int alignment, int size)
{
    void *buffer = (void*) aligned_alloc (alignment, size);
    memset (buffer, 0, size);

    return buffer;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                               TRANSLATE TARGETS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define TARGET CURR_IR_NODE.imm_value

void handleJmpTargetsX64 (X64_code *x64_code, IR *ir, Jmp_table *jmp_table, 
                          char *in_addr, char *out_addr                    )
{
    printf ("-- handleJmpTargetsX64\n\n");

    for(int i = 0; i < ir->size; i++) // ir->size == jmp_table
    {
        CURR_POS = CURR_IR_NODE.x64_pos;

        switch(CURR_IR_NODE.command)
        {
            case IN:
            case OUT:
                handleIOAddress (x64_code, ir->buffer[i], in_addr, out_addr);
                break;
            CONDITIONAL_JMP_CASE
            case JMP:
            case CALL:
                translateTargetPtr (x64_code, ir->buffer[i], jmp_table);
                break;
            default:
                break; 
            // else skip this command -> not jump type (nothing to translate here)
        }   
    }
}

//-----------------------------------------------------------------------------

void handleIOAddress (X64_code *x64_code, IR_node ir_node, 
                      char *in_addr, char *out_addr       )
{
    if(ir_node.command == IN)
    {
        uint32_t ptr = (uint64_t) double_scanf - 
                       (uint64_t)(CURR_POS + POS_IN + SIZE_OF_PTR); 
        CURR_POS += POS_IN;

        writePtr (ptr);

        // IN IS NOT SUPPORTED IN ELF MODE
    }
    else // OUT
    {
        #ifdef ELF_MODE
        uint32_t ptr = (uint64_t) out_addr - 
                       (uint64_t)(CURR_POS - x64_code->buffer + TEXT_ADDR + POS_OUT + SIZE_OF_PTR); 
        #else
        uint32_t ptr = (uint64_t) double_printf - 
                       (uint64_t)(CURR_POS + POS_OUT + SIZE_OF_PTR); 
        #endif

        CURR_POS += POS_OUT;

        writePtr (ptr);
    }
}

//-----------------------------------------------------------------------------

void translateTargetPtr (X64_code *x64_code, IR_node ir_node, Jmp_table *jmp_table)
{
    uint64_t ptr_pos = 0;

    switch(ir_node.command)
    {
        CONDITIONAL_JMP_CASE
            ptr_pos = POS_CONDITIONAL_JUMP;
            break;
        case JMP:
            ptr_pos = POS_JUMP;
            break;
        case CALL:
            ptr_pos = POS_CALL;
            break;
    }

    uint32_t ptr = (uint64_t) jmp_table->buffer[ir_node.imm_value] - 
                   (uint64_t)(CURR_POS + ptr_pos + SIZE_OF_PTR); 
    CURR_POS += ptr_pos;

    writePtr (ptr);
}

//-----------------------------------------------------------------------------

#undef TARGET
#undef CURR_IR_NODE

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                WRITING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void writeCode_(X64_code *x64_code, uint64_t value, const char *name, int size)
{
    memcpy (x64_code->curr_pos, &value, size);
    dumpCode (x64_code, name, size);

    x64_code->curr_pos += size;
    x64_code->size     += size;  

    #ifndef ELF_MODE
    if(x64_code->size + X64_CODE_SIZE_DIFF > x64_code->capacity)
    {
        x64CodeResize (x64_code);
    }
    #endif
}

//-----------------------------------------------------------------------------

void dumpCode (X64_code *x64_code, const char *name, int size)
{
    fprintf (x64_code->dump_file, "%-25s | ", name);

    for(int i = 0; i < size; i++)
    {
        uint32_t num = (uint32_t) (uint8_t) x64_code->curr_pos[i];
        fprintf (x64_code->dump_file, "%02X ", num);
    }

    fprintf (x64_code->dump_file, "\n");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                              SPECIAL UTILS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void double_printf (double *value)
{
    printf("%0.1lf\n\n", *value);
}

void double_scanf (double *value)
{
    scanf ("%lf", value);
}

void troll_dump ()
{
    printf ("-- HELLO DUMP!\n\n");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                              TRANSLATIONS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void translateCmd (X64_code *x64_code, IR_node *curr_node)
{
    switch(curr_node->command)
    {
        case HLT:
            translateHlt (x64_code, curr_node);
            break;
        case PUSH:
            translatePush (x64_code, curr_node);
            break;
        case POP:
            translatePop (x64_code, curr_node);
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
            translateArithmOperations (x64_code, curr_node);
            break;
        case OUT:
            translateOut (x64_code, curr_node);
            break;
        case IN:
            translateIn (x64_code, curr_node);
            break;
        case DUMP:
            translateDump (x64_code, curr_node);
            break; 
        case JBE:
        case JAE:
        case JA:
        case JB:
        case JE:
        case JNE:
            translateConditionalJmps (x64_code, curr_node);
            break;
        case JMP:
            translateJmp (x64_code, curr_node);
            break;
        case CALL:
            translateCall (x64_code, curr_node);
            break;
        case RET:
            translateRet (x64_code, curr_node);
            break;
        case SQRT:
        case SIN:
        case COS:
            translateMathFunctions (x64_code, curr_node);
            break;
        default:
            printf ("UNKNOWN COMMAND!\n\n");
            break;
    }
}

//-----------------------------------------------------------------------------

void translateReg (IR_node *curr_node)
{
    // this function translate IR register into X86-64 register masks

    switch(curr_node->reg_value)
    {
        case IR_RAX:
            curr_node->reg_value = MASK_RAX;
            break; 
        case IR_RBX:
            curr_node->reg_value = MASK_RBX;
            break; 
        case IR_RCX:
            curr_node->reg_value = MASK_RCX;
            break; 
        case IR_RDX:
            curr_node->reg_value = MASK_RDX;
            break; 
        default:
            printf ("UNKNOWN REG VALUE IN IR!\n\n");
            break;
    }
}

//-----------------------------------------------------------------------------

void translateHlt (X64_code *x64_code, IR_node *curr_node)
{
    #ifdef ELF_MODE
    // mov rax, 0x3c
    writeMaskingOp (OP_MOV_REG_IMM, MASK_RAX);
    writeInt64 (0x3C);

    // xor rdi, rdi
    writeMaskingOp (OP_MOV_REG_IMM, MASK_RDI);
    writeInt64 (0x0);

    // syscall
    writeSimpleOp (OP_SYSCALL);

    #else
    writeMaskingOp (OP_PUSH_REG, MASK_R10);
    writeSimpleOp  (OP_RET);
    #endif
}

//-----------------------------------------------------------------------------

void calculateRamAddrPushPop (X64_code *x64_code, IR_node *curr_node)
{
    // put imm64 value into R13 (R13 is tmp register)
    writeMaskingOp (OP_MOV_REG_IMM, MASK_R13);

    // put int if it will used in ram access 
    uint64_t num = (uint64_t) curr_node->imm_value;
    writeInt64 (num);
    
    // I store all numbers in double representation 
    // So firstly I need to translate register value to int
    if(curr_node->reg_value)
    {   
        // support PUSH/POP [reg + imm]
        translateReg (curr_node);

        // save double value of register
        writeMaskingOp (OP_PUSH_REG, curr_node->reg_value); 

        writeMaskingOp (OP_PUSH_REG, curr_node->reg_value);  // push reg
        writeSimpleOp (OP_MOV_XMM0_STK); // pop xmm0
        writeMaskingOp (OP_ADD_REG_IMM, MASK_RSP);
        writeInt32 (8);

        // cvttsd2si reg, xmm0 <- translate double to int
        writeMaskingOp (OP_CVTTSD2SI_REG, curr_node->reg_value);

        writeMaskingOp (OP_ADD_R13_REG, curr_node->reg_value); // add r13, r_x

        // load double value of register
        writeMaskingOp (OP_POP_REG, curr_node->reg_value);
    }

    // shl is used here because of imm's size: push/pop [a] <=> push/pop [8*a]
    // I had relative addressing in my assembler and processor
    writeMaskingOp (OP_SHL_REG, MASK_R13); 
    writeByte (3); // shl reg, 3 <=> mul reg, 8

    //                                 data address
    //                                     |
    //                                     V
    // in result I operate with ram cell  r12[r13]
    writeMaskingOp (OP_ADD_R13_REG, MASK_R12); // add r13, data address

    //--------------------------------
    // So I store final address in R13
    //--------------------------------
}

//-----------------------------------------------------------------------------

void translatePush (X64_code *x64_code, IR_node *curr_node)
{  
    // I have 2 different types of push that have different purposes:
    // push from ram
    // push reg + imm
    if(curr_node->ram_flag)
    {
        translatePushRam (x64_code, curr_node);
    }

    else 
    {
        translatePushRegImm (x64_code, curr_node);
    } 
}

//-----------------------------------------------------------------------------

void translatePushRam (X64_code *x64_code, IR_node *curr_node)
{
    calculateRamAddrPushPop (x64_code, curr_node);

    writeSimpleOp (OP_MOV_R13_RAM); // mov r13, [r13]

    // and pushing it into stack
    writeMaskingOp (OP_PUSH_REG, MASK_R13);  // push r13
}

//-----------------------------------------------------------------------------

void translatePushRegImm (X64_code *x64_code, IR_node *curr_node)
{
    writeMaskingOp (OP_MOV_REG_IMM, MASK_R13);

    // put double value if it is just num
    double num = curr_node->imm_value;
    writeDouble (num);

    if(curr_node->reg_value)
    {
        // support PUSH reg + imm
        translateReg (curr_node);
        writeMaskingOp (OP_ADD_R13_REG, curr_node->reg_value); // add r13, reg
    }

    writeMaskingOp (OP_PUSH_REG, MASK_R13);  // push r13
}

//-----------------------------------------------------------------------------

void translatePop (X64_code *x64_code, IR_node *curr_node)
{
    // separate into two occasions (similar to push)
    if(curr_node->ram_flag)
    {
        translatePopRam (x64_code, curr_node);
    }

    // pop [reg + val]
    else
    {
        translatePopReg (x64_code, curr_node);
    }
}

//-----------------------------------------------------------------------------

void translatePopRam (X64_code *x64_code, IR_node *curr_node)
{
    calculateRamAddrPushPop (x64_code, curr_node);
        
    // pull out from stack num value
    writeSimpleOp (OP_MOV_XMM0_STK); // pop xmm0
    writeMaskingOp (OP_ADD_REG_IMM, MASK_RSP);
    writeInt32 (8);

    // mov it to ram
    writeSimpleOp (OP_MOV_MEM_XMM0);
}

//-----------------------------------------------------------------------------

void translatePopReg (X64_code *x64_code, IR_node *curr_node)
{
    if(curr_node->reg_value)
    {
        translateReg (curr_node);

        // pull out num from stack and store in register
        writeMaskingOp (OP_MOV_REG_STK, curr_node->reg_value);
    }

    // pop 
    writeMaskingOp (OP_ADD_REG_IMM, MASK_RSP);
    writeInt32 (8);
}

//-----------------------------------------------------------------------------

void translateArithmOperations (X64_code *x64_code, IR_node *curr_node)
{
    // pull out numbers from stack and do arithm operation
    // arithm xmm1, xmm0
    writeSimpleOp (OP_MOV_XMM0_STK); // pop xmm0
    writeSimpleOp (OP_MOV_XMM1_STK); // pop xmm1

    switch(curr_node->command)
    {
        case ADD:
            writeSimpleOp (OP_ADDSD_XMM1_XMM0);
            break;
        case SUB:
            writeSimpleOp (OP_SUBSD_XMM1_XMM0);
            break;
        case MUL:
            writeSimpleOp (OP_MULSD_XMM1_XMM0);
            break;
        case DIV:
            writeSimpleOp (OP_DIVSD_XMM1_XMM0);
            break;
        default:
            printf ("UNKNOWN COMMAND!\n");
            break;
    }

    // save only one value (second is not useful anymore)
    writeMaskingOp (OP_ADD_REG_IMM, MASK_RSP);
    writeInt32 (8);

    writeSimpleOp (OP_MOV_STK_XMM1);  // push xmm1   
}

//-----------------------------------------------------------------------------

void translateIn (X64_code *x64_code, IR_node *curr_node)
{
    // reserve 8 bytes for input number
    writeMaskingOp (OP_SUB_REG_IMM, MASK_RSP);
    writeInt32 (8);

    // save ptr in rdi (as first argument of function)
    writeSimpleOp (OP_LEA_RDI_STK_ARG);
    writePrologue (x64_code);

    writeSimpleOp (OP_CALL);

    // save bytes for unfilled target
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr for now
    writeEpilogue (x64_code);
}

//-----------------------------------------------------------------------------

void translateOut (X64_code *x64_code, IR_node *curr_node)
{
    #ifdef ELF_MODE
    writeSimpleOp (OP_MOV_XMM0_STK); // pop xmm0
    writeMaskingOp (OP_PUSH_REG, MASK_RAX); 

    // cvttsd2si reg, xmm0 <- translate double to int
    writeMaskingOp (OP_CVTTSD2SI_REG, MASK_RAX);
    writePrologue (x64_code);

    writeSimpleOp (OP_CALL);

    // save bytes for unfilled target
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr for now
    writeEpilogue (x64_code);
    writeMaskingOp (OP_POP_REG, MASK_RAX); 

    // pull out outputed number
    writeMaskingOp (OP_ADD_REG_IMM, MASK_RSP);
    writeInt32 (8);

    #else
    // save ptr in rdi (as first argument of function)
    writeSimpleOp (OP_LEA_RDI_STK_ARG);
    writePrologue (x64_code);

    writeSimpleOp (OP_CALL);

    // save bytes for unfilled target
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr for now
    writeEpilogue (x64_code);

    // pull out outputed number
    writeMaskingOp (OP_ADD_REG_IMM, MASK_RSP);
    writeInt32 (8);
    #endif
}

//-----------------------------------------------------------------------------

void translateDump (X64_code *x64_code, IR_node *curr_node)
{
    writePrologue (x64_code);

    // You can change troll_print on your own purposes
    writeSimpleOp (OP_CALL);
    uint32_t funct_ptr = (uint64_t) troll_dump - (uint64_t)(x64_code->curr_pos + SIZE_OF_PTR); 
    writePtr (funct_ptr);

    writeEpilogue (x64_code);

    // DUMP IS NOT SUPPORTED IN ELF
}

//-----------------------------------------------------------------------------

void translateConditionalJmps (X64_code *x64_code, IR_node *curr_node)
{
    // pull out numbers from stack
    writeSimpleOp (OP_MOV_XMM0_STK); 
    writeSimpleOp (OP_MOV_XMM1_STK); 

    // compare them
    writeSimpleOp (OP_CMP_XMM0_XMM1); 

    // select right conditional mask
    switch(curr_node->command)
    {
        // this cringe because of form of my J__ instruction:
        // JA -> jb ... 
        case JAE:
            writeMaskingJmp (MASK_JBE);
            break;
        case JBE:
            writeMaskingJmp (MASK_JAE);
            break;
        case JA:
            writeMaskingJmp (MASK_JB);    
            break;
        case JB:
            writeMaskingJmp (MASK_JA);
            break;
        case JE:
            writeMaskingJmp (MASK_JE);
            break;
        case JNE:
            writeMaskingJmp (MASK_JNE);
            break;
        default:
            printf ("UNKNOWN COMMAND!\n");
            break;
    }
    
    // save bytes for unfilled target
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr for now
}

//-----------------------------------------------------------------------------

void translateJmp (X64_code *x64_code, IR_node *curr_node)
{
    writeSimpleOp (OP_JMP); 

    // save bytes for unfilled target
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr
}

//-----------------------------------------------------------------------------

void translateCall (X64_code *x64_code, IR_node *curr_node)
{
    writeSimpleOp (OP_CALL);

    // save bytes for unfilled target
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr
}

//-----------------------------------------------------------------------------

void translateRet (X64_code *x64_code, IR_node *curr_node)
{
    writeSimpleOp (OP_RET);
}

//-----------------------------------------------------------------------------

void translateMathFunctions (X64_code *x64_code, IR_node *curr_node)
{
    // there is only one supported math function: SQRT
    writeSimpleOp (OP_MOV_XMM0_STK);  // pop  xmm0
    writeSimpleOp (OP_SQRTPD_XMM0);   // sqrt xmm0
    writeSimpleOp (OP_MOV_STK_XMM0);  // push xmm0 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                DUMPING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Create header in dump file (some useful information about code will represent here)

void CodeX64DumpHeader (X64_code *x64_code)
{
    printf ("-- dump x64 code\n\n");

    fprintf (x64_code->dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                          X64 TRANSLATED BIN CODE                            \n"
            "-----------------------------------------------------------------------------\n\n");

    fprintf (x64_code->dump_file, "- x64 buffer capacity: %d\n\n", x64_code->capacity);
}

//-----------------------------------------------------------------------------

void jmpTableDump (Jmp_table *jmp_table)
{
    printf ("-- dump jump table\n\n");

    FILE *dump_file = fopen ("binary_translator/dump/jump_table.txt", "w+");

    fprintf (dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                               JUMP TABLE                                    \n"
            "-----------------------------------------------------------------------------\n\n");

    fprintf (dump_file, "- jump table size: %d\n\n", jmp_table->size);

    for(int i = 0; i < jmp_table->size; i++)
    {
        fprintf (dump_file, "%p\n", jmp_table->buffer[i]);
    }

    fclose (dump_file);
}

//-----------------------------------------------------------------------------
