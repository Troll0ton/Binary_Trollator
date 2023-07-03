#include "binary_translator/include/translator.h" 
#include "binary_translator/include/executable_utils.h" 

//-----------------------------------------------------------------------------

#define CURR_IR_NODE ir->buffer[i]

X64_code *translateIrToX64 (IR *ir, int bin_size, FILE *log_file)
{
    char cmd_dump_name[MAX_LEN_OF_LINE] = { 0 };

    log_print ("- translate from IR to x64 . . . .\n\n");

    X64_code *x64_code = x64CodeCtor (X64_CODE_INIT_SIZE, PAGE_SIZE, log_file);
    x64DumpHeader (x64_code, log_file);
                                                                                 
    Memory *memory = memoryCtor (PAGE_SIZE, PAGE_SIZE, log_file);            // so we can store only MEMORY_SIZE / 8 nums in memory
    
    saveDataAddress (x64_code, memory->buffer, log_file);                      // save absolute address of RAM in R12 

    #ifndef ELF_MODE
    char op_name[MAX_LEN_OF_LINE] = { 0 };
    uint64_t mask = makeRegMask (POP_REG, R10_ID);
    writeCode (POP_REG, mask);                                                 // save return address
    #endif
                                                                                  
    for(int i = 0; i < ir->size; i++)                                          // now translate IR to opcodes and fill jmp table
    {   
        CURR_IR_NODE.address.x64 = x64_code->curr_pos;

        translateCmd (x64_code, &CURR_IR_NODE, log_file);                      // translate command
    }
    
    handleX64JmpTargets (x64_code, ir, log_file); 

    memoryDtor (memory);

    log_print ("- bin translation complete!\n\n");

    return x64_code;
}

//-----------------------------------------------------------------------------

X64_code *x64CodeCtor (int init_size, int alignment, FILE *log_file)
{
    X64_code *x64_code = (X64_code*) calloc (1, sizeof (X64_code));
    checkAlloc (x64_code);

    x64_code->buffer   = (char*) alignedCalloc (PAGE_SIZE, init_size, log_file); 
    x64_code->curr_pos = x64_code->buffer;
    x64_code->capacity = init_size;                                            // Because of unknowing of the final size, the buffer is self-expanding 

    x64_code->dump_file = fopen ("binary_translator/dump/x64_dump.txt", "w+");
    checkFilePtr (x64_code->dump_file);

    return x64_code;
}

//-----------------------------------------------------------------------------

void x64CodeResize (X64_code *x64_code, FILE *log_file)
{
    char *new_buffer = (char*) alignedCalloc (PAGE_SIZE,                       // There is no analogue of realloc for aligned buffers, so I simply copy old buffer into the new
                                              x64_code->capacity + X64_CODE_INCREASE_PAR, 
                                              log_file                                   ); 

    memcpy (new_buffer, x64_code->buffer, x64_code->capacity);
    free (x64_code->buffer);

    x64_code->buffer = new_buffer;
    x64_code->capacity += X64_CODE_INCREASE_PAR;                                // new_size = old_size + X64_CODE_INCREASE_PAR
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

Memory *memoryCtor (int size, int alignment, FILE *log_file)
{
    Memory *memory = (Memory*) calloc (1, sizeof (Memory));
    checkAlloc (memory);

    memory->buffer = (char*) alignedCalloc (PAGE_SIZE, size, log_file);
    checkAlloc (memory->buffer);

    memory->size = size;

    return memory;
}

//-----------------------------------------------------------------------------

void memoryDtor (Memory *memory)
{
    memory->size = DELETED;

    free (memory->buffer);
    free (memory);
}

//-----------------------------------------------------------------------------

void writePrologue (X64_code *x64_code, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    writeCode (PUSHA, 0);                                                      // create lite version of stack's frame

    mask = makeRegMask (PUSH_REG, RBP_ID);
    writeCode (PUSH_REG, mask);

    writeCode (MOV_RBP_RSP, 0);
    writeCode (ALIGN_STK, 0); 

    mask = makeRegMask (SUB_REG_IMM, RSP_ID);
    writeCode  (SUB_REG_IMM, mask);
    writeValue (8, SIZE_OF_NUM);

    mask = makeRegMask (PUSH_REG, RBP_ID);
    writeCode (PUSH_REG, mask);
}

//-----------------------------------------------------------------------------

void writeEpilogue (X64_code *x64_code, FILE *log_file) 
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    mask = makeRegMask (POP_REG, RBP_ID);
    writeCode (POP_REG, mask);
    
    writeCode (MOV_RSP_RBP, 0);

    mask = makeRegMask (POP_REG, RBP_ID);
    writeCode (POP_REG, mask);

    writeCode (POPA, 0);
}

//-----------------------------------------------------------------------------
                                                                     
void saveDataAddress (X64_code *x64_code, char *memory, FILE *log_file)        // r12 <- ptr to 'memory' (troll-code representation)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    mask = makeRegMask (MOV_REG_IMM, R12_ID);
    writeCode (MOV_REG_IMM, mask);

    #ifdef ELF_MODE                                                            // make correct addressing 
    uint64_t abs_ptr = (uint64_t)(x64_code->buffer + MEMORY_ADDRESS - TEXT_ADDR); 
    #else
    uint64_t abs_ptr = (uint64_t)(memory); 
    #endif

    writeValue (abs_ptr, SIZE_OF_ABS_PTR);
}

//-----------------------------------------------------------------------------

void *alignedCalloc (int alignment, int size, FILE *log_file)
{
    void *buffer = (void*) aligned_alloc (alignment, size);
    checkAlloc (buffer);

    memset (buffer, 0, size);

    return buffer;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                               TRANSLATE TARGETS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define TARGET CURR_IR_NODE.imm_value

void handleX64JmpTargets (X64_code *x64_code, IR *ir, FILE *log_file)
{
    log_print ("    - handleX64JmpTargets\n\n");

    for(int i = 0; i < ir->size; i++) 
    {
        x64_code->curr_pos = CURR_IR_NODE.address.x64;

        switch(CURR_IR_NODE.command)
        {
            CONDITIONAL_JMP_CASE
            case JMP:
            case CALL:
                translateTargetPtr (x64_code, ir, ir->buffer[i], log_file);
                break;
            default:
                break; 
                                                                               // else skip this command -> not jump type (nothing to translate here)
        }   
    }
}

//-----------------------------------------------------------------------------

void translateTargetPtr (X64_code *x64_code, IR *ir, IR_node ir_node, FILE *log_file)
{
    uint64_t ptr_pos = 0;                                                      // ptr_pos contains target's position (relative address from beginning of given IR node)

    switch(ir_node.command)
    {
        CONDITIONAL_JMP_CASE
            ptr_pos = MOV_XMM0_STK_SIZE + 
                      MOV_XMM1_STK_SIZE + 
                      ADD_REG_IMM_SIZE +
                      4 +
                      CMP_XMM0_XMM1_SIZE +
                      CONDITIONAL_JMP_SIZE;
            break;
        case JMP:
            ptr_pos = JMP_SIZE;
            break;
        case CALL:
            ptr_pos = CALL_SIZE;
            break;
    }

    uint32_t ptr = (uint64_t) ir->buffer[ir_node.imm_val.target].address.x64 - 
                   (uint64_t)(x64_code->curr_pos + ptr_pos + SIZE_OF_PTR); 
    x64_code->curr_pos += ptr_pos;

    writeValue (ptr, SIZE_OF_PTR);
}

//-----------------------------------------------------------------------------

#undef TARGET
#undef CURR_IR_NODE

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                WRITING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void writeCode_(X64_code *x64_code, uint64_t value, const char *name, int size, FILE *log_file)
{
    memcpy   (x64_code->curr_pos, &value, size);
    dumpCode (x64_code, name, size, log_file);

    x64_code->curr_pos += size;
    x64_code->size     += size;  

    #ifndef ELF_MODE
    if(x64_code->size + X64_CODE_SIZE_DIFF > x64_code->capacity)
    {
        x64CodeResize (x64_code, log_file);
    }
    #endif
}

//-----------------------------------------------------------------------------

void dumpCode (X64_code *x64_code, const char *name, int size, FILE *log_file)
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

void doublePrintf (double *value)
{
    printf ("%0.1lf\n\n", *value);
}

void doubleScanf (double *value)
{
    scanf ("%lf", value);
}

void trollDump ()
{
    printf ("HELLO DUMP!\n\n");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                              TRANSLATIONS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void translateCmd (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    switch(curr_node->command)
    {
        case HLT:
            translateHlt (x64_code, curr_node, log_file);
            break;
        case PUSH:
            translatePush (x64_code, curr_node, log_file);
            break;
        case POP:
            translatePop (x64_code, curr_node, log_file);
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
            translateArithmOperations (x64_code, curr_node, log_file);
            break;
        case OUT:
            translateOut (x64_code, curr_node, log_file);
            break;
        case IN:
            translateIn (x64_code, curr_node, log_file);
            break;
        case DUMP:
            translateDump (x64_code, curr_node, log_file);
            break; 
        case JBE:
        case JAE:
        case JA:
        case JB:
        case JE:
        case JNE:
            translateConditionalJmps (x64_code, curr_node, log_file);
            break;
        case JMP:
            translateJmp (x64_code, curr_node, log_file);
            break;
        case CALL:
            translateCall (x64_code, curr_node, log_file);
            break;
        case RET:
            translateRet (x64_code, curr_node, log_file);
            break;
        case SQRT:
        case SIN:
        case COS:
            translateMathFunctions (x64_code, curr_node, log_file);
            break;
        default:
            log_print ("COMMON OCCASSION - UNKNOWN COMMAND!\n\n");
            break;
    }
}

//-----------------------------------------------------------------------------

void translateReg (IR_node *curr_node, FILE *log_file)                         // this function translate IR register ids into X86-64 register ids
{
    switch(curr_node->reg_num)
    {
        case IR_RAX_ID:
            curr_node->reg_num = RAX_ID;
            break; 
        case IR_RBX_ID:
            curr_node->reg_num = RBX_ID;
            break; 
        case IR_RCX_ID:
            curr_node->reg_num = RCX_ID;
            break; 
        case IR_RDX_ID:
            curr_node->reg_num = RDX_ID;
            break; 
        default:
            log_print ("UNKNOWN REG VALUE IN IR!\n\n");
            break;
    }
}

//-----------------------------------------------------------------------------

uint64_t makeRegMask_(int reg_id, int reg_id_pos, int rex_b_pos)
{
    uint64_t reg_mask  = 0;
    uint64_t rex_b_bit = 0;

    if(reg_id >= 0b1000)                                                       // handle rnums ids
    {
        reg_id -= 0b1000;
        rex_b_bit = 1;
    }

    reg_mask |= reg_id    << reg_id_pos;
    reg_mask |= rex_b_bit << rex_b_pos;

    return reg_mask;
}

//-----------------------------------------------------------------------------

uint64_t makeJmpMask (int jmp_type)
{
    uint64_t jmp_mask = 0;

    jmp_mask = jmp_type << JMP_MASK_POS;

    return jmp_mask;  
}

//-----------------------------------------------------------------------------

void translateHlt (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    #ifdef ELF_MODE
    mask = makeRegMask (MOV_REG_IMM, RAX_ID);
    writeCode  (MOV_REG_IMM, mask);                                             // mov rax, 0x3c
    writeValue (0x3C, SIZE_OF_LONG_NUM);

    mask = makeRegMask (MOV_REG_IMM, RDI_ID);                                   // xor rdi, rdi
    writeCode  (MOV_REG_IMM, mask);
    writeValue (0x0, SIZE_OF_LONG_NUM);
                                                        
    writeCode (SYSCALL, 0);                                                     // syscall

    #else
    mask = makeRegMask (PUSH_REG, R10_ID);
    writeCode (PUSH_REG, mask);

    writeCode (RET, 0);
    #endif
}

//-----------------------------------------------------------------------------

void calculateMemoryAddrPushPop (X64_code *x64_code, 
                                 IR_node  *curr_node, 
                                 FILE     *log_file  )
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };
                                                                                
    mask = makeRegMask (MOV_REG_IMM, R13_ID);                                  // put imm64 value into R13 (R13 is tmp register)
    writeCode (MOV_REG_IMM, mask);

    uint64_t num = (uint64_t) curr_node->imm_val.num;                          // put int if it will used in memory access
    writeValue (num, SIZE_OF_LONG_NUM);
                                                                               // I store all numbers in double representation 
                                                                               // So firstly I need to translate register value to int
    if(curr_node->reg_num)                                                      
    {                                  
        translateReg (curr_node, log_file);                                    // support PUSH/POP [reg + imm]
                                                                                
        mask = makeRegMask (PUSH_REG, curr_node->reg_num);                     // save double value of register
        writeCode (PUSH_REG, mask);

        mask = makeRegMask (PUSH_REG, curr_node->reg_num);                     // push reg
        writeCode (PUSH_REG, mask);  
        
        writeCode (MOV_XMM0_STK, 0);                                           // pop xmm0

        mask = makeRegMask (ADD_REG_IMM, RSP_ID);
        writeCode  (ADD_REG_IMM, mask);
        writeValue (8, SIZE_OF_NUM);

        mask = makeRegMask (CVTTSD2SI_REG, curr_node->reg_num);                // cvttsd2si reg, xmm0 <- translate double to int           
        writeCode (CVTTSD2SI_REG, mask);

        mask = makeRegMask (ADD_R13_REG, curr_node->reg_num);                  // add r13, r_x
        writeCode (ADD_R13_REG, mask); 

        mask = makeRegMask (POP_REG, curr_node->reg_num);                      // load double value of register
        writeCode (POP_REG, mask);                       
    }
                                                                               // shl is used here because of imm's size: push/pop [a] <=> push/pop [8*a]
    mask = makeRegMask (SHL_REG, R13_ID);                                      // I had relative addressing in my assembler and processor
    writeCode  (SHL_REG, mask); 
    writeValue (3, BYTE);                                                      // shl reg, 3 <=> mul reg, 8

    mask = makeRegMask (ADD_R13_REG, R12_ID);                                  // in result I operate with memory cell  r12[r13]
    writeCode (ADD_R13_REG, mask);                                             // add r13, data address
}

//-----------------------------------------------------------------------------

void translatePush (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{  
    if(curr_node->memory_flag)
    {
        translatePushMemory (x64_code, curr_node, log_file);
    }

    else 
    {
        translatePushRegImm (x64_code, curr_node, log_file);
    } 
}

//-----------------------------------------------------------------------------

void translatePushMemory (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    calculateMemoryAddrPushPop (x64_code, curr_node, log_file);

    writeCode (MOV_R13_RAM, 0);                                                // mov r13, [r13]
    
    mask = makeRegMask (PUSH_REG, R13_ID);                                     // and pushing it into stack
    writeCode (PUSH_REG, mask);                                                // push r13
}

//-----------------------------------------------------------------------------

void translatePushRegImm (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    mask = makeRegMask (MOV_REG_IMM, R13_ID);  
    writeCode  (MOV_REG_IMM, mask);
    writeValue (*(uint64_t*) &curr_node->imm_val.num, SIZE_OF_LONG_NUM);       // put double value if it is just num

    if(curr_node->reg_num)
    {
        translateReg (curr_node, log_file);                                    // support PUSH reg + imm

        mask = makeRegMask (ADD_R13_REG, curr_node->reg_num);                  // add r13, reg
        writeCode (ADD_R13_REG, mask);              
    }

    mask = makeRegMask (PUSH_REG, R13_ID);                                     // push r13
    writeCode (PUSH_REG, mask);  
}
 
//-----------------------------------------------------------------------------

void translatePop (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    if(curr_node->memory_flag)                                                 // separate into two occasions (similar to push)
    {
        translatePopMemory (x64_code, curr_node, log_file);
    }

    else                                                                       // pop [reg + val]
    {
        translatePopReg (x64_code, curr_node, log_file);
    }
}

//-----------------------------------------------------------------------------

void translatePopMemory (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    calculateMemoryAddrPushPop (x64_code, curr_node, log_file);
        
    writeCode (MOV_XMM0_STK, 0);                                               // pull out from stack num value
    
    mask = makeRegMask (ADD_REG_IMM, RSP_ID); 
    writeCode  (ADD_REG_IMM, mask);
    writeValue (8, SIZE_OF_NUM);

    writeCode (MOV_MEM_XMM0, 0);                                               // mov it to memory
}

//-----------------------------------------------------------------------------

void translatePopReg (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    if(curr_node->reg_num)
    {
        translateReg (curr_node, log_file);

        mask = makeRegMask (MOV_REG_STK, curr_node->reg_num);                  // pull out num from stack and store in register
        writeCode (MOV_REG_STK, mask);
    }

    mask = makeRegMask (ADD_REG_IMM, RSP_ID);                                  // pop 
    writeCode  (ADD_REG_IMM, mask);
    writeValue (8, SIZE_OF_NUM);
}

//-----------------------------------------------------------------------------

void translateArithmOperations (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };
                                                                               // pull out numbers from stack and do arithm operation
                                                                               // arithm xmm1, xmm0
    writeCode (MOV_XMM0_STK, 0);                                               // pop xmm0
    writeCode (MOV_XMM1_STK, 0);                                               // pop xmm1

    switch(curr_node->command)
    {
        case ADD:
            writeCode (ADDSD_XMM1_XMM0, 0);
            break;
        case SUB:
            writeCode (SUBSD_XMM1_XMM0, 0);
            break;
        case MUL:
            writeCode (MULSD_XMM1_XMM0, 0);
            break;
        case DIV:
            writeCode (DIVSD_XMM1_XMM0, 0);
            break;
        default:
            log_print ("ARITHM OPERATION - UNKNOWN COMMAND!\n");
            break;
    }

    mask = makeRegMask (ADD_REG_IMM, RSP_ID);                                  // save only one value (second is not useful anymore)                  
    writeCode  (ADD_REG_IMM, mask);
    writeValue (8, SIZE_OF_NUM);

    writeCode (MOV_STK_XMM1, 0);                                               // push xmm1   
}

//-----------------------------------------------------------------------------

void translateIn (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    mask = makeRegMask (SUB_REG_IMM, RSP_ID);                                   // reserve 8 bytes for input number
    writeCode  (SUB_REG_IMM, mask);
    writeValue (8, SIZE_OF_NUM);

    writeCode (LEA_RDI_STK_ARG, 0);                                             // save ptr in rdi (as first argument of function)
    writePrologue (x64_code, log_file);

    writeCode (CALL, 0);                                                              
    uint32_t ptr = (uint64_t) doubleScanf - (uint64_t)(x64_code->curr_pos + SIZE_OF_PTR); 
    writeValue (ptr, SIZE_OF_PTR);

    writeEpilogue (x64_code, log_file);
}

//-----------------------------------------------------------------------------

void translateOut (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    #ifdef ELF_MODE
    writeCode (MOV_XMM0_STK, 0);                                               // pop xmm0

    mask = makeRegMask (PUSH_REG, RAX_ID); 
    writeCode (PUSH_REG, mask); 

    mask = makeRegMask (CVTTSD2SI_REG, RAX_ID);                                // cvttsd2si reg, xmm0 <- translate double to int
    writeCode (CVTTSD2SI_REG, mask);

    writePrologue (x64_code, log_file);

    writeCode (CALL, 0);
                                                                               
    uint32_t ptr = (uint64_t) FUNCT_ADDR - (uint64_t)(x64_code->curr_pos - x64_code->buffer + TEXT_ADDR + SIZE_OF_PTR); 
    writeValue (ptr, SIZE_OF_PTR);

    writeEpilogue (x64_code, log_file); 

    mask = makeRegMask (POP_REG, RAX_ID);
    writeCode (POP_REG, mask); 

    mask = makeRegMask (ADD_REG_IMM, RSP_ID);                                  // pull out outputed number
    writeCode  (ADD_REG_IMM, mask);
    writeValue (8, SIZE_OF_NUM);

    #else
    writeCode (LEA_RDI_STK_ARG, 0);                                            // save ptr in rdi (as first argument of function)
    writePrologue (x64_code, log_file);

    writeCode (CALL, 0);                                                                
    uint32_t ptr = (uint64_t) doublePrintf - (uint64_t)(x64_code->curr_pos + SIZE_OF_PTR); 
    writeValue (ptr, SIZE_OF_PTR);

    writeEpilogue (x64_code, log_file); 

    mask = makeRegMask (ADD_REG_IMM, RSP_ID);                                  // pull out already printed number
    writeCode  (ADD_REG_IMM, mask);
    writeValue (8, SIZE_OF_NUM);
    #endif
}

//-----------------------------------------------------------------------------

void translateDump (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };
    
    writePrologue (x64_code, log_file);

    writeCode (CALL, 0);                                                       // You can change troll_print on your own purposes
    uint32_t funct_ptr = (uint64_t) trollDump - (uint64_t)(x64_code->curr_pos + SIZE_OF_PTR); 
    writeValue (funct_ptr, SIZE_OF_PTR);

    writeEpilogue (x64_code, log_file);                                        // DUMP IS NOT SUPPORTED IN ELF
}

//-----------------------------------------------------------------------------

void translateConditionalJmps (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    uint64_t mask = 0;
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    writeCode (MOV_XMM0_STK, 0);                                               // pull out numbers from stack
    writeCode (MOV_XMM1_STK, 0); 

    mask = makeRegMask (ADD_REG_IMM, RSP_ID);                                  // pop pop
    writeCode  (ADD_REG_IMM, mask);
    writeValue (16, SIZE_OF_NUM);

    writeCode (CMP_XMM0_XMM1, 0);                                              // compare them 

    switch(curr_node->command)                                                 // select right conditional mask
    {
        case JAE:
            mask = makeJmpMask (MASK_JBE);
            break;
        case JBE:
            mask = makeJmpMask (MASK_JAE);
            break;
        case JA:
            mask = makeJmpMask (MASK_JB);    
            break;
        case JB:
            mask = makeJmpMask (MASK_JA);
            break;
        case JE:
            mask = makeJmpMask (MASK_JE);
            break;
        case JNE:
            mask = makeJmpMask (MASK_JNE);
            break;
        default:
            log_print ("CONDITIONAL JUMP - UNKNOWN COMMAND!\n");
            break;
    }

    writeCode (CONDITIONAL_JMP, mask);
                                                                               // save bytes for unfilled target
    x64_code->curr_pos += SIZE_OF_PTR;                                         // skip ptr for now
}

//-----------------------------------------------------------------------------

void translateJmp (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    writeCode (JMP, 0); 
                                                                               // save bytes for unfilled target
    x64_code->curr_pos += SIZE_OF_PTR;                                         // skip ptr
}

//-----------------------------------------------------------------------------

void translateCall (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    writeCode (CALL, 0);
                                                                                // save bytes for unfilled target
    x64_code->curr_pos += SIZE_OF_PTR;                                          // skip ptr
}

//-----------------------------------------------------------------------------

void translateRet (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    char op_name[MAX_LEN_OF_LINE] = { 0 };

    writeCode (RET, 0);
}

//-----------------------------------------------------------------------------

void translateMathFunctions (X64_code *x64_code, IR_node *curr_node, FILE *log_file)
{
    char op_name[MAX_LEN_OF_LINE] = { 0 };
                                                                               // there is only one supported math function: SQRT
    writeCode (MOV_XMM0_STK, 0);                                               // pop  xmm0
    writeCode (SQRTPD_XMM0, 0);                                                // sqrt xmm0
    writeCode (MOV_STK_XMM0, 0);                                               // push xmm0 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                DUMPING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void x64DumpHeader (X64_code *x64_code, FILE *log_file)                        // Create header in dump file (some useful information about code will represent here)
{
    log_print ("- dump x64 code\n\n");

    fprintf (x64_code->dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                          X64 TRANSLATED BIN CODE                            \n"
            "-----------------------------------------------------------------------------\n\n");

    fprintf (x64_code->dump_file, "- x64 buffer capacity: %d\n\n", x64_code->capacity);
}

//-----------------------------------------------------------------------------
