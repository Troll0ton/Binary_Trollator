#include "binary_translator/include/translator.h" 

//-----------------------------------------------------------------------------

#define CURR_CMD ir->buffer[i]

//-----------------------------------------------------------------------------

X86_code *translateIrToX86 (IR *ir, int bin_size)
{
    printf ("-- write commands\n\n");

    X86_code *x86_code = (X86_code*) calloc (1, sizeof (X86_code));
    x86_code->buffer = (char*) aligned_alloc (PAGESIZE, ir->size * tmp_size);

    Jump_table jump_table = { 0 };
    jump_table.x86_pos = (char**) calloc (bin_size, sizeof (char*));

    char *curr_pos = x86_code->buffer;
                                     // there and in other places - SIZE of command
                                     //        |
                                     //        V        
    writeCmd (&curr_pos, (char*) &X86_MOV_R10, 2);
    writeAbsPtr (&curr_pos, (uint64_t) curr_pos);

    for(int i = 0; i < ir->size; i++)
    {
        jump_table.x86_pos[CURR_CMD.bin_pos] = curr_pos;

        translateCmd (&CURR_CMD, &curr_pos);
    }

    writeCmd (&curr_pos, (char*) &X86_RET, 1);

    x86_code->size = curr_pos - x86_code->buffer + 1;

    return x86_code;
}

#undef CURR_CMD

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                WRITING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void writeCmd (char **code, char *cmd, int size)
{
    memcpy (*code, cmd, size);

    *code += size;
}

void writePtr (char **code, uint32_t addr)
{
    memcpy (*code, &addr, SIZE_OF_PTR);

    *code += SIZE_OF_PTR;
}

void writeAbsPtr (char **code, uint64_t addr)
{
    memcpy (*code, &addr, SIZE_OF_ABS_PTR);

    *code += SIZE_OF_ABS_PTR;
}

void writeNum (char **code, double num)
{
    memcpy (*code, &num, SIZE_OF_NUM);

    *code += SIZE_OF_NUM;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                 STDIO
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int double_printf (double *value)
{
    return printf("%lg\n\n", *value);
}

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
            translateOut (curr_node, curr_pos);
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
    writeCmd (curr_pos, (char*) &X86_RET, 1);
}

//-----------------------------------------------------------------------------

void translatePush (IR_node *curr_node, char **curr_pos)
{
    printf ("HANDLE PUSH:\n");

    if(!curr_node->ram_flag)
    {
        writeCmd (curr_pos, (char*) &X86_MOV_R13, 2);  // mov r13, imm64
        writeNum (curr_pos, (double) curr_node->imm_value); 

        printf ("handled val: %lg\n", (double) curr_node->imm_value);

        if(curr_node->reg_value)
        {
            uint32_t tmp_cmd = MASK_X86_ADD_R13_R_X | 
                              (X86_ADD_R13_R_X[curr_node->reg_value - 1] << X86_ADD_R13_R_X_OFFSET);

            writeCmd (curr_pos, (char*) &(tmp_cmd), 3); // add r13, r_x
        }

        writeCmd (curr_pos, (char*) &X86_PUSH_R13, 2);  // push r13
    }

    else
    {
        //
    }
}

//-----------------------------------------------------------------------------

void translatePop (IR_node *curr_node, char **curr_pos)
{
    if(!curr_node->ram_flag)
    {
        uint32_t tmp_cmd = X86_POP_R_X[curr_node->reg_value - 1]; 
                                        // rax -> 0, rbx -> 1, ...
        writeCmd (curr_pos, (char*) &(tmp_cmd), 1);
    }

    //writeCmd (&curr_pos, (char*) &X86_ADD_RSP, 4);
}

//-----------------------------------------------------------------------------

void translateArithmOperations (IR_node *curr_node, char **curr_pos)
{
    writeCmd (curr_pos, (char*) &X86_MOV_XMM0_STK, 6); // pop xmm0
    writeCmd (curr_pos, (char*) &X86_MOV_XMM1_STK, 6); // pop xmm1

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
    }
    writeCmd (curr_pos, sel_cmd, 4);                    // op xmm1, xmm0 -> xmm1

    writeCmd (curr_pos, (char*) &X86_ADD_RSP, 4); 
    writeCmd (curr_pos, (char*) &X86_MOV_STK_XMM1, 6);  // push xmm1   
}

//-----------------------------------------------------------------------------

void translateOut (IR_node *curr_node, char **curr_pos)
{
    writeCmd (curr_pos, (char*) &X86_LEA_RDI_RSP, 4);

    writeCmd (curr_pos, (char*) &X86_PUSH_RBP, 1);
    writeCmd (curr_pos, (char*) &X86_PUSHA, 6);
    writeCmd (curr_pos, (char*) &X86_MOV_RBP_RSP, 3);

    writeCmd (curr_pos, (char*) &X86_CALL, 1);
    uint32_t out_ptr = (uint64_t) double_printf - (uint64_t)(*curr_pos + 4); 
    writePtr (curr_pos, out_ptr);

    writeCmd (curr_pos, (char*) &X86_MOV_RSP_RBP, 3);
    writeCmd (curr_pos, (char*) &X86_POPA, 6);
    writeCmd (curr_pos, (char*) &X86_POP_RBP, 1);

    writeCmd (curr_pos, (char*) &X86_ADD_RSP, 4);
}

//-----------------------------------------------------------------------------

void translateDump (IR_node *curr_node, char **curr_pos)
{
    /// ? ? ? ?
}

//-----------------------------------------------------------------------------

void translateConditionalJmps (IR_node *curr_node, char **curr_pos)
{
    /// ? ? ? ?
}

//-----------------------------------------------------------------------------

void translateJmp (IR_node *curr_node, char **curr_pos)
{
    /// ? ? ? ?
}

//-----------------------------------------------------------------------------

void translateCall (IR_node *curr_node, char **curr_pos)
{
    /// ? ? ? ?
}

//-----------------------------------------------------------------------------

void translateRet (IR_node *curr_node, char **curr_pos)
{
    /// ? ? ? ?
}

//-----------------------------------------------------------------------------

void translateMathFunctions (IR_node *curr_node, char **curr_pos)
{
    // NEED to add SIN and COS
    writeCmd (curr_pos, (char*) &X86_MOV_XMM0_STK, 6);  // pop  xmm0
    writeCmd (curr_pos, (char*) &X86_SQRTPD, 4);        // sqrt xmm0
    writeCmd (curr_pos, (char*) &X86_MOV_STK_XMM0, 6);  // push xmm0 
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
            "o o o o o o o o o o o \n\n");

    void (*execute_code) (void) = (void (*) (void)) (code);
    execute_code ();

    printf ("o o o o o o o o o o o  \n\n"
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

    for(int i = 0; i < size; i++)
    {
        uint32_t num = (uint32_t) (uint8_t) code[i];
        fprintf (dump_file, "%X ", num);
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