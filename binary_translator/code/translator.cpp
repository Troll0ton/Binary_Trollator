#include "binary_translator/include/translator.h" 

//-----------------------------------------------------------------------------

#define CURR_IR_NODE ir->buffer[i]
#define CURR_POS x64_code->curr_pos

X64_code *translateIrToX64 (IR *ir, int bin_size)
{
    printf ("-- write commands\n\n");

    X64_code  *x64_code  = x64CodeCtor  (X64_CODE_INIT_SIZE, PAGESIZE);
    Ram       *ram       = ramCtor      (RAM_INIT_SIZE,      PAGESIZE);
    Jmp_table *jmp_table = jmpTableCtor (ir->size);

    saveDataAddress (x64_code, ram->buffer);    // save absolute address to ram in R12 

    for(int i = 0; i < ir->size; i++)
    {
        jmp_table->buffer[i] = CURR_POS;        // fill jump table
        CURR_IR_NODE.x64_pos = CURR_POS;

        translateCmd (&CURR_IR_NODE, &CURR_POS);
    }

    writeCode (x64_code, OP_RET, OP_RET_SIZE);

    jmpTableDump (jmp_table);
    translateJmpTargetsX64 (ir, jmp_table);

    jmpTableDtor (jmp_table);
    ramDtor      (ram);

    return x64_code;
}

//-----------------------------------------------------------------------------

X64_code *x64CodeCtor (int init_size, int alignment)
{
    X64_code *x64_code = (X64_code*) calloc        (1,        sizeof (X64_code));
    x64_code->buffer   = (char*)     alignedCalloc (PAGESIZE, init_size);
    x64_code->curr_pos = x64_code->buffer;
    x64_code->capacity = init_size;
    //x64_code->size   = 0;

    return x64_code;
}

//-----------------------------------------------------------------------------

void x64CodeResize (X64_code *x64_code)
{
    char *new_buffer = (char*) alignedCalloc (PAGESIZE, 
                                              x64_code->capacity * X64_CODE_INCREASE_PAR); 

    memcpy (new_buffer, x64_code->buffer, x64_code->capacity);
    free   (x64_code->buffer);
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
    //ram->size = 0;

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

    return jmp_table;
}

//-----------------------------------------------------------------------------

void jmpTableDtor (Jmp_table *jmp_table)
{
    jmp_table->size = DELETED;

    free (jmp_table->buffer);
    free (jmp_table);
}

//-----------------------------------------------------------------------------

void writePrologue (X64_code *x64_code)
{
    writeCode (x64_code, OP_PUSH_REG | MASK_RBP << PUSH_POP_REG_POS, OP_PUSH_REG_SIZE);
    writeCode (x64_code, OP_PUSHA, OP_PUSHA_SIZE);
    writeCode (x64_code, OP_MOV_RBP_RSP, OP_MOV_RBP_RSP_SIZE);
}

void writeEpilogue (X64_code *x64_code) 
{
    writeCode (x64_code, OP_MOV_RSP_RBP, OP_MOV_RSP_RBP_SIZE);
    writeCode (x64_code, OP_POPA, OP_POPA_SIZE);
    writeCode (x64_code, OP_POP_REG | MASK_RBP << PUSH_POP_REG_POS, OP_POP_REG_SIZE);
}

//-----------------------------------------------------------------------------

void saveDataAddress (X64_code *x64_code, char *ram) // r12 <- ptr to 'ram' (troll-code representation)
{
    writeCode (x64_code, OP_MOV_REG_IMM | MASK_R12 << PUSH_POP_REG_POS | MASK_R, OP_MOV_REG_IMM_SIZE);

    uint64_t ptr = (uint64_t)(ram); 
    writeCode (x64_code, ptr, SIZE_OF_ABS_PTR);
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
        uint64_t curr_offset = 0;

        switch(CURR_IR_NODE.command)
        {
            CONDITIONAL_JMP_CASE
            case JMP:
            case CALL:
                translateTargetPtr (x64_code, ir->buffer[i], jmp_table);
            default:
                break; // else skip this command -> not jump type
        }   
    }
}

//-----------------------------------------------------------------------------

void translateTargetPtr (X64_code *x64_code, IR_node ir_node, Jmp_table *jmp_table)
{
    uint64_t curr_offset = 0;

    switch(ir_node.command)
    {
        CONDITIONAL_JMP_CASE
            curr_offset = X64_CONDITIONAL_JUMP_OFFSET;
            break;
        case JMP:
            curr_offset = X64_JUMP_OFFSET;
            break;
        case CALL:
            curr_offset = X64_CALL_OFFSET;
            break;
    }

    uint32_t ptr = (uint64_t) jmp_table->buffer[ir_node.imm_value] - 
                   (uint64_t)(CURR_POS + curr_offset + SIZE_OF_PTR); 

    CURR_POS += X64_CALL_OFFSET;

    writeCode (x64_code, ptr, SIZE_OF_PTR);
}

//-----------------------------------------------------------------------------

#undef TARGET
#undef CURR_POS
#undef CURR_IR_NODE

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                WRITING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void writeCode (X64_code *x64_code, uint64_t value, int size)
{
    memcpy (x64_code->curr_pos, &size, size);
    x64_code->curr_pos += size;
    x64_code->size     += size;  

    if(x64_code->size + X64_CODE_SIZE_DIFF > x64_code->capacity)
    {
        x64CodeResize (x64_code);
    }
}

void writeDouble (X64_code *x64_code, double num)
{
    memcpy (x64_code->curr_pos, &num, SIZE_OF_NUM);
    x64_code->curr_pos += SIZE_OF_NUM;
    x64_code->size     += SIZE_OF_NUM;  

    if(x64_code->size + X64_CODE_SIZE_DIFF > x64_code->capacity)
    {
        x64CodeResize (x64_code);
    }
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
//-----------------------------------------------------------------------------
//                              TRANSLATIONS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void translateCmd (IR_node *curr_node, char **curr_pos)
{
    switch(curr_node->command)
    {
        case HLT:
            translateHlt (curr_node, curr_pos);
            break;
        case PUSH:
            translatePush (curr_node, curr_pos);
            break;
        case POP:
            translatePop (curr_node, curr_pos);
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
            translateArithmOperations (curr_node, curr_pos);
            break;
        case OUT:
        case IN:
            translateStdio (curr_node, curr_pos);
            break;
        case DUMP:
            translateDump (curr_node, curr_pos);
            break; 
        case JBE:
        case JAE:
        case JA:
        case JB:
        case JE:
        case JNE:
            translateConditionalJmps (curr_node, curr_pos);
            break;
        case JMP:
            translateJmp (curr_node, curr_pos);
            break;
        case CALL:
            translateCall (curr_node, curr_pos);
            break;
        case RET:
            translateRet (curr_node, curr_pos);
            break;
        case SQRT:
        case SIN:
        case COS:
            translateMathFunctions (curr_node, curr_pos);
            break;
    }
}

//-----------------------------------------------------------------------------

void translateHlt (IR_node *curr_node, char **curr_pos)
{
    writeCode (curr_pos, X64_RET);
}

//-----------------------------------------------------------------------------

void translatePush (IR_node *curr_node, char **curr_pos)
{
    writeCode (curr_pos, X64_MOV_R13);  // mov r13, imm64

    if(!curr_node->ram_flag)
    {
        writeDouble (curr_pos, (double) curr_node->imm_value);
    }

    else 
    {
        writeInt (curr_pos, (long long int) curr_node->imm_value);
    } 

    if(curr_node->reg_value)
    {
        uint32_t tmp_cmd = MASK_X64_ADD_R13_R_X | 
                          (X64_R_X[curr_node->reg_value - ir_reg_mask] 
                        << X64_ADD_R13_R_X_OFFSET);

        writeCode (curr_pos, (char*) &(tmp_cmd), 3); // add r13, r_x
    }

    if(curr_node->ram_flag)
    {
        writeCode (curr_pos, X64_MUL_R13_8);   // mul r13, 8
        writeCode (curr_pos, X64_ADD_R13_R12); // add r13, data address
        writeCode (curr_pos, X64_MOV_R13_R13); // mov r13, [r13]
    }

    writeCode (curr_pos, X64_PUSH_R13);  // push r13
}

//-----------------------------------------------------------------------------

void translatePop (IR_node *curr_node, char **curr_pos)
{
    if(!curr_node->ram_flag)
    {
        if(curr_node->reg_value)
        {
            uint32_t tmp_cmd = X64_MOV_R_X_STK | 
                              (X64_R_X[curr_node->reg_value - ir_reg_mask] 
                            << X64_MOV_R_X_STK_MASK_LEN);

            writeCode (curr_pos, (char*) &tmp_cmd, X64_MOV_R_X_STK_SIZE);
            writeCode (curr_pos, X64_ADD_RSP);
        }

        else
        {
            writeCode (curr_pos, X64_ADD_RSP); // pop 
        } 
    }

    else
    {
        writeCode (curr_pos, X64_MOV_XMM0_STK); // pop xmm0
        writeCode (curr_pos, X64_ADD_RSP);
        writeCode (curr_pos, X64_MOV_R13);  // mov r13, imm64

        if(!curr_node->ram_flag)
        {
            writeDouble (curr_pos, (double) curr_node->imm_value);
        }

        else 
        {
            writeInt (curr_pos, (long long int) curr_node->imm_value);
        } 

        if(curr_node->reg_value)
        {
            uint32_t tmp_cmd = MASK_X64_ADD_R13_R_X | 
                              (X64_R_X[curr_node->reg_value - ir_reg_mask] 
                            << X64_ADD_R13_R_X_OFFSET);

            writeCode (curr_pos, (char*) &(tmp_cmd), MASK_X64_ADD_R13_R_X_SIZE); // add r13, r_x
        }

        writeCode (curr_pos, X64_MUL_R13_8);   // mul r13, 8
        writeCode (curr_pos, X64_ADD_R13_R12);
        writeCode (curr_pos, X64_MOV_MEM_XMM0);
    }
}

//-----------------------------------------------------------------------------

void translateArithmOperations (IR_node *curr_node, char **curr_pos)
{
    writeCode (curr_pos, X64_MOV_XMM0_STK); // pop xmm0
    writeCode (curr_pos, X64_MOV_XMM1_STK); // pop xmm1

    char *sel_cmd = NULL;

    switch(curr_node->command)
    {
        case ADD:
            sel_cmd = (char*) &X64_ADDSD;
            break;
        case SUB:
            sel_cmd = (char*) &X64_SUBSD;
            break;
        case MUL:
            sel_cmd = (char*) &X64_MULSD;
            break;
        case DIV:
            sel_cmd = (char*) &X64_DIVSD;
            break;
        default:
            printf ("UNKNOWN COMMAND!\n");
            break;
    }
    writeCode (curr_pos, sel_cmd, X64_OPSD_SIZE); // op xmm1, xmm0 -> xmm1

    writeCode (curr_pos, X64_ADD_RSP); 
    writeCode (curr_pos, X64_MOV_STK_XMM1);  // push xmm1   
}

//-----------------------------------------------------------------------------

void translateStdio (IR_node *curr_node, char **curr_pos)
{
    if(curr_node->command == IN)
    {
        writeCode (curr_pos, X64_SUB_RSP);
    }

    writeCode (curr_pos, X64_LEA_RDI_RSP);

    writePrologue (curr_pos);
    writeCode (curr_pos, X64_ALIGN_STK); 

    writeCode (curr_pos, X64_CALL);
    uint64_t ptr = 0;

    if(curr_node->command == OUT)
    {
        ptr = (uint64_t) double_printf;
    }

    else
    {
        ptr = (uint64_t) double_scanf;
    }

    ptr -= (uint64_t)(*curr_pos + SIZE_OF_PTR); 
    writeCode (curr_pos, (uint32_t) ptr);

    writeEpilogue (curr_pos);

    if(curr_node->command == OUT)
    {
        writeCode (curr_pos, X64_ADD_RSP);
    }
}

//-----------------------------------------------------------------------------

void translateDump (IR_node *curr_node, char **curr_pos)
{
    writePrologue (curr_pos);

    // You can change troll_print on your purposes
    writeCode (curr_pos, X64_CALL);
    uint32_t ptr = (uint64_t) troll_dump - (uint64_t)(*curr_pos + SIZE_OF_PTR); 
    writeCode (curr_pos, ptr);

    writeEpilogue (curr_pos);
}

//-----------------------------------------------------------------------------

void translateConditionalJmps (IR_node *curr_node, char **curr_pos)
{
    writeCode (curr_pos, X64_MOV_XMM0_STK); 
    writeCode (curr_pos, X64_MOV_XMM1_STK); 
    writeCode (curr_pos, X64_CMP_XMM0_XMM1); 

    uint32_t curr_cmd = X64_MASK_JMP;
    
    switch(curr_node->command)
    {
        case JBE:
            curr_cmd |= (X64_MASK_JBE << JMP_MASK_LEN);
            break;
        case JAE:
            curr_cmd |= (X64_MASK_JAE << JMP_MASK_LEN);
            break;
        case JA:
            curr_cmd |= (X64_MASK_JA << JMP_MASK_LEN);    
            break;
        case JB:
            curr_cmd |= (X64_MASK_JB << JMP_MASK_LEN);
            break;
        case JE:
            curr_cmd |= (X64_MASK_JE << JMP_MASK_LEN);
            break;
        case JNE:
            curr_cmd |= (X64_MASK_JNE << JMP_MASK_LEN);
            break;
        default:
            printf ("UNKNOWN COMMAND!\n");
            break;
    }
    
    writeCode (curr_pos, (char*) &curr_cmd, X64_MASK_JMP_SIZE);
    *curr_pos += SIZE_OF_PTR; // skip ptr for now
}

//-----------------------------------------------------------------------------

void translateJmp (IR_node *curr_node, char **curr_pos)
{
    writeCode (curr_pos, X64_JMP); 
    *curr_pos += SIZE_OF_PTR;
}

//-----------------------------------------------------------------------------

void translateCall (IR_node *curr_node, char **curr_pos)
{
    writePrologue (curr_pos);
    writeCode (curr_pos, X64_ALIGN_STK);

    writeCode (curr_pos, X64_CALL);
    *curr_pos += SIZE_OF_PTR; // skip ptr
}

//-----------------------------------------------------------------------------

void translateRet (IR_node *curr_node, char **curr_pos)
{
    writeCode (curr_pos, X64_RET);

    writeEpilogue (curr_pos);
}

//-----------------------------------------------------------------------------

void translateMathFunctions (IR_node *curr_node, char **curr_pos)
{
    // NEED to add SIN and COS
    writeCode (curr_pos, X64_MOV_XMM0_STK);  // pop  xmm0
    writeCode (curr_pos, X64_SQRTPD);        // sqrt xmm0
    writeCode (curr_pos, X64_MOV_STK_XMM0);  // push xmm0 
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

void CodeX64Dump (char *code, int size)
{
    printf ("-- dump x64 code\n\n");

    FILE *dump_file = fopen ("binary_translator/dump/x64_dump.txt", "w+");

    fprintf (dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                          X64 TRANSLATED BIN CODE                            \n"
            "-----------------------------------------------------------------------------\n\n");

    fprintf (dump_file, "- x64 translated code size: %d\n\n", size);

    int line_len = 0;

    for(int i = 0; i < size; i++)
    {
        if(line_len > max_len_dump)
        {
            fprintf (dump_file, "\n\n");
            line_len = 0;
        }

        uint32_t num = (uint32_t) (uint8_t) code[i];
        fprintf (dump_file, "%02X ", num);
        line_len++;
    }

    fclose (dump_file);
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
