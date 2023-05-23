#include "binary_translator/include/translator.h" 

//-----------------------------------------------------------------------------

#define CURR_CMD ir->buffer[i]

//-----------------------------------------------------------------------------

X86_code *translateIrToX86 (IR *ir, int bin_size)
{
    printf ("-- write commands\n\n");

    X86_code *x86_code = (X86_code*) calloc (1, sizeof (X86_code));
    x86_code->buffer = (char*) aligned_calloc (PAGESIZE, X86_CODE_SIZE);
    char *memory = (char*) aligned_calloc (PAGESIZE, MEMORY_SIZE);

    char **jump_table = (char**) calloc (ir->size, sizeof (char*));

    char *curr_pos = x86_code->buffer;

    saveDataAddress (&curr_pos, memory);

    for(int i = 0; i < ir->size; i++)
    {
        jump_table[i] = curr_pos;
        CURR_CMD.x86_pos = curr_pos;

        translateCmd (&CURR_CMD, &curr_pos);
    }

    writeCmd_(&curr_pos, X86_RET);

    jumpTableDump (jump_table, ir->size);
    translateJmpTargetsX86 (ir, jump_table);

    x86_code->size = curr_pos - x86_code->buffer + 1;

    free (jump_table);
    free (memory);

    return x86_code;
}

//-----------------------------------------------------------------------------

void writePrologue (char **curr_pos)
{
    writeCmd_(curr_pos, X86_PUSH_RBP);
    writeCmd_(curr_pos, X86_PUSHA);
    writeCmd_(curr_pos, X86_MOV_RBP_RSP);
}

void writeEpilogue (char **curr_pos)
{
    writeCmd_(curr_pos, X86_MOV_RSP_RBP);
    writeCmd_(curr_pos, X86_POPA);
    writeCmd_(curr_pos, X86_POP_RBP);
}

//-----------------------------------------------------------------------------

void saveDataAddress (char **curr_pos, char *memory) // r12 <- ptr to 'ram' (troll-code representation)
{
    writeCmd_(curr_pos, X86_MOV_R12);

    uint64_t ptr = (uint64_t)(memory); 
    writeAbsPtr (curr_pos, ptr);
}

//-----------------------------------------------------------------------------

void *aligned_calloc (int alignment, int size)
{
    void *buffer = (void*) aligned_alloc (PAGESIZE, size);
    memset (buffer, 0, size);
    return buffer;
}

//-----------------------------------------------------------------------------

#define TARGET CURR_CMD.imm_value

void translateJmpTargetsX86 (IR *ir, char **jump_table)
{
    for(int i = 0; i < ir->size; i++)
    {
        char *curr_pos = CURR_CMD.x86_pos;

        if(IS_CONDITION_JUMP (CURR_CMD.command))
        {
            uint32_t ptr = (uint64_t) jump_table[TARGET] - 
                               (uint64_t)(X86_CONDITIONAL_JUMP_OFFSET + SIZE_OF_PTR); 

            curr_pos += X86_CONDITIONAL_JUMP_OFFSET;

            writePtr (&curr_pos, ptr);
        }

        else if(CURR_CMD.command == JMP)
        {
            uint32_t ptr = (uint64_t) jump_table[TARGET] - 
                               (uint64_t)(curr_pos + X86_JUMP_OFFSET + SIZE_OF_PTR); 

            curr_pos += X86_JUMP_OFFSET;

            writePtr (&curr_pos, ptr);
        }

        else if(CURR_CMD.command == CALL)
        {
            uint32_t ptr = (uint64_t) jump_table[TARGET] - 
                               (uint64_t)(curr_pos + X86_CALL_OFFSET + SIZE_OF_PTR); 

            curr_pos += X86_CALL_OFFSET;

            writePtr (&curr_pos, ptr);
        }

        // else skip this command -> not jump type
    }
}

#undef TARGET

#undef CURR_CMD

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                WRITING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void writeCmd (char **curr_pos, char *cmd, int size)
{
    memcpy (*curr_pos, cmd, size);

    *curr_pos += size;
}

void writePtr (char **curr_pos, uint32_t addr)
{
    memcpy (*curr_pos, &addr, SIZE_OF_PTR);

    *curr_pos += SIZE_OF_PTR;
}

void writeAbsPtr (char **curr_pos, uint64_t addr)
{
    memcpy (*curr_pos, &addr, SIZE_OF_ABS_PTR);

    *curr_pos += SIZE_OF_ABS_PTR;
}

void writeDouble (char **curr_pos, double num)
{
    memcpy (*curr_pos, &num, SIZE_OF_NUM);

    *curr_pos += SIZE_OF_NUM;
}

void writeInt (char **curr_pos, uint64_t num)
{
    memcpy (*curr_pos, &num, SIZE_OF_NUM);

    *curr_pos += SIZE_OF_NUM;
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
    writeCmd_(curr_pos, X86_RET);
}

//-----------------------------------------------------------------------------

void translatePush (IR_node *curr_node, char **curr_pos)
{
    writeCmd_(curr_pos, X86_MOV_R13);  // mov r13, imm64

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
        uint32_t tmp_cmd = MASK_X86_ADD_R13_R_X | 
                          (X86_R_X[curr_node->reg_value - ir_reg_mask] 
                        << X86_ADD_R13_R_X_OFFSET);

        writeCmd (curr_pos, (char*) &(tmp_cmd), 3); // add r13, r_x
    }

    if(curr_node->ram_flag)
    {
        writeCmd_(curr_pos, X86_MUL_R13_8);   // mul r13, 8
        writeCmd_(curr_pos, X86_ADD_R13_R12); // add r13, data address
        writeCmd_(curr_pos, X86_MOV_R13_R13); // mov r13, [r13]
    }

    writeCmd_(curr_pos, X86_PUSH_R13);  // push r13
}

//-----------------------------------------------------------------------------

void translatePop (IR_node *curr_node, char **curr_pos)
{
    if(!curr_node->ram_flag)
    {
        if(curr_node->reg_value)
        {
            uint32_t tmp_cmd = X86_MOV_R_X_STK | 
                              (X86_R_X[curr_node->reg_value - ir_reg_mask] 
                            << X86_MOV_R_X_STK_MASK_LEN);

            writeCmd (curr_pos, (char*) &tmp_cmd, X86_MOV_R_X_STK_SIZE);
            writeCmd_(curr_pos, X86_ADD_RSP);
        }

        else
        {
            writeCmd_(curr_pos, X86_ADD_RSP); // pop 
        } 
    }

    else
    {
        writeCmd_(curr_pos, X86_MOV_XMM0_STK); // pop xmm0
        writeCmd_(curr_pos, X86_ADD_RSP);
        writeCmd_(curr_pos, X86_MOV_R13);  // mov r13, imm64

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
            uint32_t tmp_cmd = MASK_X86_ADD_R13_R_X | 
                              (X86_R_X[curr_node->reg_value - ir_reg_mask] 
                            << X86_ADD_R13_R_X_OFFSET);

            writeCmd (curr_pos, (char*) &(tmp_cmd), MASK_X86_ADD_R13_R_X_SIZE); // add r13, r_x
        }

        writeCmd_(curr_pos, X86_MUL_R13_8);   // mul r13, 8
        writeCmd_(curr_pos, X86_ADD_R13_R12);
        writeCmd_(curr_pos, X86_MOV_MEM_XMM0);
    }
}

//-----------------------------------------------------------------------------

void translateArithmOperations (IR_node *curr_node, char **curr_pos)
{
    writeCmd_(curr_pos, X86_MOV_XMM0_STK); // pop xmm0
    writeCmd_(curr_pos, X86_MOV_XMM1_STK); // pop xmm1

    char *sel_cmd = NULL;

    switch(curr_node->command)
    {
        case ADD:
            sel_cmd = (char*) &X86_ADDSD;
            break;
        case SUB:
            sel_cmd = (char*) &X86_SUBSD;
            break;
        case MUL:
            sel_cmd = (char*) &X86_MULSD;
            break;
        case DIV:
            sel_cmd = (char*) &X86_DIVSD;
            break;
        default:
            printf ("UNKNOWN COMMAND!\n");
            break;
    }
    writeCmd (curr_pos, sel_cmd, X86_OPSD_SIZE); // op xmm1, xmm0 -> xmm1

    writeCmd_(curr_pos, X86_ADD_RSP); 
    writeCmd_(curr_pos, X86_MOV_STK_XMM1);  // push xmm1   
}

//-----------------------------------------------------------------------------

void translateStdio (IR_node *curr_node, char **curr_pos)
{
    if(curr_node->command == IN)
    {
        writeCmd_(curr_pos, X86_SUB_RSP);
    }

    writeCmd_(curr_pos, X86_LEA_RDI_RSP);

    writePrologue (curr_pos);
    writeCmd_(curr_pos, X86_ALIGN_STK); 

    writeCmd_(curr_pos, X86_CALL);
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
    writePtr (curr_pos, (uint32_t) ptr);

    writeEpilogue (curr_pos);

    if(curr_node->command == OUT)
    {
        writeCmd_(curr_pos, X86_ADD_RSP);
    }
}

//-----------------------------------------------------------------------------

void translateDump (IR_node *curr_node, char **curr_pos)
{
    writePrologue (curr_pos);

    // You can change troll_print on your purposes
    writeCmd_(curr_pos, X86_CALL);
    uint32_t ptr = (uint64_t) troll_dump - (uint64_t)(*curr_pos + SIZE_OF_PTR); 
    writePtr (curr_pos, ptr);

    writeEpilogue (curr_pos);
}

//-----------------------------------------------------------------------------

void translateConditionalJmps (IR_node *curr_node, char **curr_pos)
{
    writeCmd_(curr_pos, X86_MOV_XMM0_STK); 
    writeCmd_(curr_pos, X86_MOV_XMM1_STK); 
    writeCmd_(curr_pos, X86_CMP_XMM0_XMM1); 

    uint32_t curr_cmd = X86_MASK_JMP;
    
    switch(curr_node->command)
    {
        case JBE:
            curr_cmd |= (X86_MASK_JBE << JMP_MASK_LEN);
            break;
        case JAE:
            curr_cmd |= (X86_MASK_JAE << JMP_MASK_LEN);
            break;
        case JA:
            curr_cmd |= (X86_MASK_JA << JMP_MASK_LEN);    
            break;
        case JB:
            curr_cmd |= (X86_MASK_JB << JMP_MASK_LEN);
            break;
        case JE:
            curr_cmd |= (X86_MASK_JE << JMP_MASK_LEN);
            break;
        case JNE:
            curr_cmd |= (X86_MASK_JNE << JMP_MASK_LEN);
            break;
        default:
            printf ("UNKNOWN COMMAND!\n");
            break;
    }
    
    writeCmd (curr_pos, (char*) &curr_cmd, X86_MASK_JMP_SIZE);
    *curr_pos += SIZE_OF_PTR; // skip ptr for now
}

//-----------------------------------------------------------------------------

void translateJmp (IR_node *curr_node, char **curr_pos)
{
    writeCmd_(curr_pos, X86_JMP); 
    *curr_pos += SIZE_OF_PTR;
}

//-----------------------------------------------------------------------------

void translateCall (IR_node *curr_node, char **curr_pos)
{
    writePrologue (curr_pos);
    writeCmd_(curr_pos, X86_ALIGN_STK);

    writeCmd_(curr_pos, X86_CALL);
    *curr_pos += SIZE_OF_PTR; // skip ptr
}

//-----------------------------------------------------------------------------

void translateRet (IR_node *curr_node, char **curr_pos)
{
    writeCmd_(curr_pos, X86_RET);

    writeEpilogue (curr_pos);
}

//-----------------------------------------------------------------------------

void translateMathFunctions (IR_node *curr_node, char **curr_pos)
{
    // NEED to add SIN and COS
    writeCmd_(curr_pos, X86_MOV_XMM0_STK);  // pop  xmm0
    writeCmd_(curr_pos, X86_SQRTPD);        // sqrt xmm0
    writeCmd_(curr_pos, X86_MOV_STK_XMM0);  // push xmm0 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                RUNNING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void runCode (char *code, int size)
{
    int mprotect_status = mprotect (code, size, PROT_READ | PROT_WRITE | PROT_EXEC);

    if(mprotect_status == mprotect_error)
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

void CodeX86Dump (char *code, int size)
{
    printf ("-- dump x86 code\n\n");

    FILE *dump_file = fopen ("binary_translator/dump/x86_dump.txt", "w+");

    fprintf (dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                          X86 TRANSLATED BIN CODE                            \n"
            "-----------------------------------------------------------------------------\n\n");

    fprintf (dump_file, "- x86 translated code size: %d\n\n", size);

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

void jumpTableDump (char **jump_table, int size)
{
    printf ("-- dump jump table\n\n");

    FILE *dump_file = fopen ("binary_translator/dump/jump_table.txt", "w+");

    fprintf (dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                               JUMP TABLE                                    \n"
            "-----------------------------------------------------------------------------\n\n");

    fprintf (dump_file, "- jump table size: %d\n\n", size);

    for(int i = 0; i < size; i++)
    {
        fprintf (dump_file, "%p\n", jump_table[i]);
    }

    fclose (dump_file);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                             CTORS AND DTORS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void X86RepresentDtor (X86_code *x86_code)
{
    free (x86_code->buffer);
    x86_code->size = deleted;
}

//-----------------------------------------------------------------------------
