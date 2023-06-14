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

    writeSimpleCode (OP_RET);

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
    writeMaskingCode (OP_PUSH_REG, MASK_RBP, POS_MASK_REG, 0);
    writeSimpleCode  (OP_PUSHA);
    writeSimpleCode  (OP_MOV_RBP_RSP);
}

//-----------------------------------------------------------------------------

void writeEpilogue (X64_code *x64_code) 
{
    writeSimpleCode  (OP_MOV_RSP_RBP);
    writeSimpleCode  (OP_POPA);
    writeMaskingCode (OP_POP_REG, MASK_RBP, POS_MASK_REG, 0);
}

//-----------------------------------------------------------------------------

void saveDataAddress (X64_code *x64_code, char *ram) // r12 <- ptr to 'ram' (troll-code representation)
{
    writeMaskingCode (OP_MOV_REG_IMM, MASK_R12, POS_MASK_REG, MASK_R);

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

void writeCode (X64_code *x64_code, uint64_t value, char *name, int size)
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

void dumpCode (X64_code *x64_code, char *name, int size)
{
    fprintf (x64_code->dump_file, "%-40s | ", name);

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
    writeSimpleCode (OP_RET);
}

//-----------------------------------------------------------------------------

void translatePush (X64_code *x64_code, IR_node *curr_node)
{
    writeMaskingCode (OP_MOV_REG_IMM, MASK_R13, POS_MASK_REG, MASK_R);

    if(!curr_node->ram_flag)
    {
        double num = curr_node->imm_value;
        writeNum (num);
    }

    else 
    {
        uint64_t num = curr_node->imm_value;
        writeNum (num);
    } 

    if(curr_node->reg_value)
    {
        translateReg (curr_node);
        writeMaskingCode (OP_ADD_R13_REG, curr_node->reg_value, POS_ADD_R13_REG, 
                             OP_ADD_R13_REG_SIZE                                      ); // add r13, r_x
    }

    if(curr_node->ram_flag)
    {
        writeCode (x64_code, OP_SHL_REG | MASK_R13 << POS_SHL_REG | MASK_R, OP_SHL_REG_SIZE);   // mul r13, 8
        writeCode (x64_code, 8, BYTE);

        writeCode (x64_code, OP_ADD_R13_REG | MASK_R12 << POS_ADD_R13_REG, 
                             OP_ADD_R13_REG_SIZE                          ); // add r13, data address

        writeCode (x64_code, OP_MOV_R13_RAM, OP_MOV_R13_RAM_SIZE); // mov r13, [r13]
    }

    writeCode (x64_code, OP_PUSH_REG | MASK_R13 << POS_MASK_REG | MASK_R, OP_PUSH_REG_SIZE);  // push r13
}

//-----------------------------------------------------------------------------

void translatePop (X64_code *x64_code, IR_node *curr_node)
{
    if(!curr_node->ram_flag)
    {
        if(curr_node->reg_value)
        {
            translateReg (curr_node);
            writeCode (x64_code, OP_MOV_REG_STK | curr_node->reg_value << POS_MOV_REG_STK, 
                                 OP_MOV_REG_STK_SIZE);
        }

        writeCode (x64_code, OP_ADD_REG_IMM | MASK_RSP << POS_ADD_REG_IMM, OP_ADD_REG_IMM_SIZE);
        writeCode (x64_code, 8, SIZE_OF_NUM);
    }

    else
    {
        writeCode (x64_code, OP_MOV_XMM0_STK, OP_MOV_XMM0_STK_SIZE); // pop xmm0
        writeCode (x64_code, OP_ADD_REG_IMM | MASK_RSP << POS_ADD_REG_IMM, OP_ADD_REG_IMM_SIZE);
        writeCode (x64_code, 8, SIZE_OF_NUM);

        writeCode (x64_code, OP_MOV_REG_IMM | MASK_R13 << POS_MASK_REG | MASK_R, OP_MOV_REG_IMM_SIZE);

        if(!curr_node->ram_flag)
        {
            writeDouble (x64_code, (double) curr_node->imm_value);
        }

        else 
        {
            writeCode (x64_code, (uint64_t) curr_node->imm_value, SIZE_OF_NUM);
        } 

        if(curr_node->reg_value)
        {
            translateReg (curr_node);
            writeCode (x64_code, OP_ADD_R13_REG | curr_node->reg_value << POS_ADD_R13_REG, 
                                 OP_ADD_R13_REG_SIZE                                      ); // add r13, r_x
        }

        writeCode (x64_code, OP_SHL_REG | MASK_R13 << POS_SHL_REG | MASK_R, OP_SHL_REG_SIZE);   // mul r13, 8
        writeCode (x64_code, 8, BYTE);
        
        writeCode (x64_code, OP_ADD_R13_REG | MASK_R12 << POS_ADD_R13_REG, 
                             OP_ADD_R13_REG_SIZE                          ); // add r13, data address

        writeCode (x64_code, OP_MOV_MEM_XMM0, OP_MOV_MEM_XMM0_SIZE);
    }
}

//-----------------------------------------------------------------------------

void translateArithmOperations (X64_code *x64_code, IR_node *curr_node)
{
    writeCode (x64_code, OP_MOV_XMM0_STK, OP_MOV_XMM0_STK_SIZE); // pop xmm0
    writeCode (x64_code, OP_MOV_XMM1_STK, OP_MOV_XMM1_STK_SIZE); // pop xmm1

    uint64_t op_code = 0;

    switch(curr_node->command)
    {
        case ADD:
            op_code = OP_ADDSD_XMM1_XMM0;
            break;
        case SUB:
            op_code = OP_SUBSD_XMM1_XMM0;
            break;
        case MUL:
            op_code = OP_MULSD_XMM1_XMM0;
            break;
        case DIV:
            op_code = OP_DIVSD_XMM1_XMM0;
            break;
        default:
            printf ("UNKNOWN COMMAND!\n");
            break;
    }
    writeCode (x64_code, op_code, OP_ARITHM_SIZE); // op xmm1, xmm0 -> xmm1

    writeCode (x64_code, OP_ADD_REG_IMM | MASK_RSP << POS_ADD_REG_IMM, OP_ADD_REG_IMM_SIZE);
    writeCode (x64_code, 8, SIZE_OF_NUM);

    writeCode (x64_code, OP_MOV_STK_XMM1, OP_MOV_STK_XMM1_SIZE);  // push xmm1   
}

//-----------------------------------------------------------------------------

void translateStdio (X64_code *x64_code, IR_node *curr_node)
{
    if(curr_node->command == IN)
    {
        writeCode (x64_code, OP_SUB_REG_IMM | MASK_RSP << POS_ADD_REG_IMM, OP_SUB_REG_IMM_SIZE);
        writeCode (x64_code, 8, SIZE_OF_NUM);
    }

    writeCode (x64_code, OP_LEA_RDI_STK_ARG, OP_LEA_RDI_STK_ARG_SIZE);

    writePrologue (x64_code);
    writeCode (x64_code, OP_ALIGN_STK, OP_ALIGN_STK_SIZE); 
    writeCode (x64_code, OP_CALL, OP_CALL_SIZE);

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
    writeCode (x64_code, (uint32_t) funct_ptr, SIZE_OF_PTR);

    writeEpilogue (x64_code);

    if(curr_node->command == OUT)
    {
        writeCode (x64_code, OP_ADD_REG_IMM | MASK_RSP << POS_ADD_REG_IMM, OP_ADD_REG_IMM_SIZE);
        writeCode (x64_code, 8, SIZE_OF_NUM);
    }
}

//-----------------------------------------------------------------------------

void translateDump (X64_code *x64_code, IR_node *curr_node)
{
    writePrologue (x64_code);

    // You can change troll_print on your purposes
    writeCode (x64_code, OP_CALL, OP_CALL_SIZE);
    uint32_t funct_ptr = (uint64_t) troll_dump - (uint64_t)(x64_code->curr_pos + SIZE_OF_PTR); 
    writeCode (x64_code, funct_ptr, SIZE_OF_PTR);

    writeEpilogue (x64_code);
}

//-----------------------------------------------------------------------------

void translateConditionalJmps (X64_code *x64_code, IR_node *curr_node)
{
    writeCode (x64_code, OP_MOV_XMM0_STK,  OP_MOV_XMM0_STK_SIZE); 
    writeCode (x64_code, OP_MOV_XMM1_STK,  OP_MOV_XMM1_STK_SIZE); 
    writeCode (x64_code, OP_CMP_XMM0_XMM1, OP_CMP_XMM0_XMM1_SIZE); 

    uint64_t op_code = OP_CONDITIONAL_JMP;
    
    switch(curr_node->command)
    {
        case JAE:
            op_code |= MASK_JAE << POS_MASK_JMP;
            break;
        case JBE:
            op_code |= MASK_JBE << POS_MASK_JMP;
            break;
        case JA:
            op_code |= MASK_JA << POS_MASK_JMP;    
            break;
        case JB:
            op_code |= MASK_JB << POS_MASK_JMP;
            break;
        case JE:
            op_code |= MASK_JE << POS_MASK_JMP;
            break;
        case JNE:
            op_code |= MASK_JNE << POS_MASK_JMP;
            break;
        default:
            printf ("UNKNOWN COMMAND!\n");
            break;
    }
    
    writeCode (x64_code, op_code, OP_CONDITIONAL_JMP_SIZE);
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr for now
}

//-----------------------------------------------------------------------------

void translateJmp (X64_code *x64_code, IR_node *curr_node)
{
    writeCode (x64_code, OP_JMP, OP_JMP_SIZE); 
    x64_code->curr_pos += SIZE_OF_PTR;
}

//-----------------------------------------------------------------------------

void translateCall (X64_code *x64_code, IR_node *curr_node)
{
    writePrologue (x64_code);
    writeCode (x64_code, OP_ALIGN_STK, OP_ALIGN_STK_SIZE);

    writeCode (x64_code, OP_CALL, OP_CALL_SIZE);
    x64_code->curr_pos += SIZE_OF_PTR; // skip ptr
}

//-----------------------------------------------------------------------------

void translateRet (X64_code *x64_code, IR_node *curr_node)
{
    writeCode (x64_code, OP_RET, OP_RET_SIZE);
    writeEpilogue (x64_code);
}

//-----------------------------------------------------------------------------

void translateMathFunctions (X64_code *x64_code, IR_node *curr_node)
{
    writeCode (x64_code, OP_MOV_XMM0_STK, OP_MOV_XMM0_STK_SIZE);  // pop  xmm0
    writeCode (x64_code, OP_SQRTPD_XMM0, OP_ARITHM_SIZE);         // sqrt xmm0
    writeCode (x64_code, OP_MOV_STK_XMM0, OP_MOV_STK_XMM0_SIZE);  // push xmm0 
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
