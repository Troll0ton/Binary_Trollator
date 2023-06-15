#include "binary_translator/include/translator.h" 

//-----------------------------------------------------------------------------

#define CURR_IR_NODE ir->buffer[i]

X64_code *translateIrToX64 (IR *ir, int bin_size)
{
    printf ("-- write commands\n\n");

    X64_code *x64_code = x64CodeCtor (X64_CODE_INIT_SIZE, PAGESIZE);
    CodeX64DumpHeader (x64_code);

    Ram       *ram       = ramCtor      (RAM_INIT_SIZE, PAGESIZE);
    Jmp_table *jmp_table = jmpTableCtor (ir->size);

    saveDataAddress (x64_code, ram->buffer);    // save absolute address to ram in R12 

    for(int i = 0; i < ir->size; i++)
    {
        jmp_table->buffer[i] = CURR_POS;        // fill jump table
        CURR_IR_NODE.x64_pos = CURR_POS;

        translateCmd (x64_code, &CURR_IR_NODE);
    }

    writeSimpleOp (RET);

    jmpTableDump (jmp_table);
    handleJmpTargetsX64 (x64_code, ir, jmp_table);

    jmpTableDtor (jmp_table);
    ramDtor (ram);

    return x64_code;
}

//-----------------------------------------------------------------------------

X64_code *x64CodeCtor (int init_size, int alignment)
{
    X64_code *x64_code  = (X64_code*) calloc        (1,        sizeof (X64_code));
    x64_code->buffer    = (char*)     alignedCalloc (PAGESIZE, init_size);
    x64_code->curr_pos  = x64_code->buffer; 
    x64_code->capacity  = init_size;
    x64_code->dump_file = fopen ("binary_translator/dump/x64_dump.txt", "w+");

    return x64_code;
}

//-----------------------------------------------------------------------------

void x64CodeResize (X64_code *x64_code)
{
    char *new_buffer = (char*) alignedCalloc (PAGESIZE, 
                                              x64_code->capacity * X64_CODE_INCREASE_PAR); 

    memcpy (new_buffer, x64_code->buffer, x64_code->capacity);
    free (x64_code->buffer);

    x64_code->buffer = new_buffer;

    x64_code->capacity *= X64_CODE_INCREASE_PAR;
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
    ram->buffer = (char*) alignedCalloc (PAGESIZE, size);
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
    free (jmp_table->buffer);
    free (jmp_table);
    fclose (jmp_table->dump_file);
}

//-----------------------------------------------------------------------------

void writePrologue (X64_code *x64_code)
{
    writeMaskingOp (PUSH_REG, MASK_RBP);
    writeSimpleOp  (PUSHA);
    writeSimpleOp  (MOV_RBP_RSP);
}

//-----------------------------------------------------------------------------

void writeEpilogue (X64_code *x64_code) 
{
    writeSimpleOp  (MOV_RSP_RBP);
    writeSimpleOp  (POPA);
    writeMaskingOp (POP_REG, MASK_RBP);
}

//-----------------------------------------------------------------------------

void saveDataAddress (X64_code *x64_code, char *ram) // r12 <- ptr to 'ram' (troll-code representation)
{
    writeMaskingOp (MOV_REG_IMM, MASK_R12);

    uint64_t ptr = (uint64_t)(ram); 
    writeAbsPtr (ptr);
}

//-----------------------------------------------------------------------------

void *alignedCalloc (int alignment, int size)
{
    void *buffer = (void*) aligned_alloc (alignment, size);
    memset (buffer, 0, size);

    return buffer;
}

//-----------------------------------------------------------------------------

#define TARGET CURR_IR_NODE.imm_value

void handleJmpTargetsX64 (X64_code *x64_code, IR *ir, Jmp_table *jmp_table)
{
    for(int i = 0; i < ir->size; i++) // ir->size == jmp_table
    {
        CURR_POS = CURR_IR_NODE.x64_pos;

        switch(CURR_IR_NODE.command)
        {
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

void translateTargetPtr (X64_code *x64_code, IR_node ir_node, Jmp_table *jmp_table)
{
    uint64_t opcode_offset = 0;

    switch(ir_node.command)
    {
        CONDITIONAL_JMP_CASE
            opcode_offset = X64_CONDITIONAL_JUMP_OFFSET;
            break;
        case JMP:
            opcode_offset = X64_JUMP_OFFSET;
            break;
        case CALL:
            opcode_offset = X64_CALL_OFFSET;
            break;
    }

    uint32_t ptr = (uint64_t) jmp_table->buffer[ir_node.imm_value] - 
                   (uint64_t)(CURR_POS + opcode_offset + SIZE_OF_PTR); 

    CURR_POS += opcode_offset;

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

    if(x64_code->size + X64_CODE_SIZE_DIFF > x64_code->capacity)
    {
        x64CodeResize (x64_code);
    }
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

    fprintf (x64_code->dump_file, "%\n");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                              SPECIAL UTILS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void double_printf (double *value)
{
    printf("%lg\n\n", *value);
}

void double_scanf (double *value)
{
    scanf ("%lg", value);
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
        case IN:
            translateStdio (x64_code, curr_node);
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
    }
}

//-----------------------------------------------------------------------------

void translateReg (IR_node *curr_node)
{
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
    writeSimpleOp (RET);
}

//-----------------------------------------------------------------------------

void translatePush (X64_code *x64_code, IR_node *curr_node)
{
    writeMaskingOp (MOV_REG_IMM, MASK_R13);

    if(!curr_node->ram_flag)
    {
        double num = curr_node->imm_value;
        writeDouble (num);
    }

    else 
    {
        uint64_t num = curr_node->imm_value;
        writeInt (num);
    } 

    if(curr_node->reg_value)
    {
        translateReg (curr_node);
        writeMaskingOp (ADD_R13_REG, curr_node->reg_value); // add r13, r_x
    }

    if(curr_node->ram_flag)
    {
        writeMaskingOp (SHL_REG, MASK_R13); // shl r13, 8
        writeByte (8);

        writeMaskingOp (ADD_R13_REG, MASK_R12); // add r13, data address
        writeSimpleOp (MOV_R13_RAM); // mov r13, [r13]
    }

    writeMaskingOp (PUSH_REG, MASK_R13);  // push r13
}

//-----------------------------------------------------------------------------

void translatePop (X64_code *x64_code, IR_node *curr_node)
{
    if(!curr_node->ram_flag)
    {
        if(curr_node->reg_value)
        {
            translateReg (curr_node);
            writeMaskingOp (MOV_REG_STK, curr_node->reg_value);
        }

        writeMaskingOp (ADD_REG_IMM, MASK_RSP);
        writeInt (8);
    }

    else
    {
        writeSimpleOp (MOV_XMM0_STK); // pop xmm0
        writeMaskingOp (ADD_REG_IMM, MASK_RSP);
        writeInt (8);

        writeMaskingOp (MOV_REG_IMM, MASK_R13);

        if(!curr_node->ram_flag)
        {
            double num = curr_node->imm_value;
            writeDouble (num);
        }

        else 
        {
            uint64_t num = curr_node->imm_value;
            writeInt (num);
        } 

        if(curr_node->reg_value)
        {
            translateReg (curr_node);
            writeMaskingOp (ADD_R13_REG, curr_node->reg_value); // add r13, r_x
        }

        writeMaskingOp (SHL_REG, MASK_R13);   // mul r13, 8
        writeByte (8);
        
        writeMaskingOp (ADD_R13_REG, MASK_R12); // add r13, data address
        writeSimpleOp (MOV_MEM_XMM0);
    }
}

//-----------------------------------------------------------------------------

void translateArithmOperations (X64_code *x64_code, IR_node *curr_node)
{
    writeSimpleOp (MOV_XMM0_STK); // pop xmm0
    writeSimpleOp (MOV_XMM1_STK); // pop xmm1

    switch(curr_node->command)
    {
        case ADD:
            writeSimpleOp (ADDSD_XMM1_XMM0);
            break;
        case SUB:
            writeSimpleOp (SUBSD_XMM1_XMM0);
            break;
        case MUL:
            writeSimpleOp (MULSD_XMM1_XMM0);
            break;
        case DIV:
            writeSimpleOp (DIVSD_XMM1_XMM0);
            break;
        default:
            printf ("UNKNOWN COMMAND!\n");
            break;
    }

    writeMaskingOp (ADD_REG_IMM, MASK_RSP);
    writeInt (8);

    writeSimpleOp (MOV_STK_XMM1);  // push xmm1   
}

//-----------------------------------------------------------------------------

void translateStdio (X64_code *x64_code, IR_node *curr_node)
{
    if(curr_node->command == IN)
    {
        writeMaskingOp (SUB_REG_IMM, MASK_RSP);
        writeInt (8);
    }

    writeSimpleOp (LEA_RDI_STK_ARG);

    writePrologue (x64_code);

    writeSimpleOp (ALIGN_STK); 
    writeSimpleOp (CALL);

    uint64_t funct_ptr = 0;

    switch(curr_node->command)
    {
        case IN:
            funct_ptr = (uint64_t) double_scanf;
            break;
        case OUT:
            funct_ptr = (uint64_t) double_printf;
            break;
        default:
            printf ("UNKNOWN STDIO FUNCTION\n\n");
            break;    
    }

    funct_ptr -= (uint64_t)(x64_code->curr_pos + SIZE_OF_PTR); 
    writePtr (funct_ptr);

    writeEpilogue (x64_code);

    if(curr_node->command == OUT)
    {
        writeMaskingOp (ADD_REG_IMM, MASK_RSP);
        writeInt (8);
    }
}

//-----------------------------------------------------------------------------

void translateDump (X64_code *x64_code, IR_node *curr_node)
{
    writePrologue (x64_code);

    // You can change troll_print on your own purposes
    writeSimpleOp (CALL);
    uint32_t funct_ptr = (uint64_t) troll_dump - (uint64_t)(x64_code->curr_pos + SIZE_OF_PTR); 
    writePtr (funct_ptr);

    writeEpilogue (x64_code);
}

//-----------------------------------------------------------------------------

void translateConditionalJmps (X64_code *x64_code, IR_node *curr_node)
{
    writeSimpleOp (MOV_XMM0_STK); 
    writeSimpleOp (MOV_XMM1_STK); 
    writeSimpleOp (CMP_XMM0_XMM1); 

    switch(curr_node->command)
    {
        case JAE:
            writeMaskingJmp (MASK_JAE);
            break;
        case JBE:
            writeMaskingJmp (MASK_JBE);
            break;
        case JA:
            writeMaskingJmp (MASK_JA);    
            break;
        case JB:
            writeMaskingJmp (MASK_JB);
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
    
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr for now
}

//-----------------------------------------------------------------------------

void translateJmp (X64_code *x64_code, IR_node *curr_node)
{
    writeSimpleOp (JMP); 
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr
}

//-----------------------------------------------------------------------------

void translateCall (X64_code *x64_code, IR_node *curr_node)
{
    writePrologue (x64_code);
    writeSimpleOp (ALIGN_STK);
    writeSimpleOp (CALL);

    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr
}

//-----------------------------------------------------------------------------

void translateRet (X64_code *x64_code, IR_node *curr_node)
{
    writeSimpleOp (RET);
    writeEpilogue (x64_code);
}

//-----------------------------------------------------------------------------

void translateMathFunctions (X64_code *x64_code, IR_node *curr_node)
{
    writeSimpleOp (MOV_XMM0_STK);  // pop  xmm0
    writeSimpleOp (SQRTPD_XMM0);   // sqrt xmm0
    writeSimpleOp (MOV_STK_XMM0);  // push xmm0 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                RUNNING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void runCode (char *code, int size)
{
    int mprotect_status = mprotect (code, size, PROT_READ | PROT_WRITE | PROT_EXEC);

    if(mprotect_status == MPROTECT_ERROR)
    {
        perror("mprotect error:"); // added not cringe error handling
    }

    printf ("-- executing...       \n\n"
            "   o o o o o o o o o  \n\n");

    void (*execute_code) (void) = (void (*) (void)) (code);
    execute_code ();

    printf ("   o o o o o o o o o  \n\n"
            "-- executing completed!\n\n");
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
