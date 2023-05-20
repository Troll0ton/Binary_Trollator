
WRITE_CMD(HLT, "hlt",
{
    writeCmd (&curr_pos, (char*) &X86_RET, 1);
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(PUSH, "push",
{
    if(!CURR_CMD.ram_flag)
    {
        writeCmd (&curr_pos, (char*) &X86_MOV_R13, 2);  // mov r13, imm64
        writeNum (&curr_pos, (double) num); 

        if(CURR_CMD.reg_value)
        {
            uint32_t tmp_cmd = MASK_X86_ADD_R13_R_X | 
                              (X86_ADD_R13_R_X[CURR_CMD.reg_value - 1] << X86_ADD_R13_R_X_OFFSET);

            writeCmd (&curr_pos, (char*) &(tmp_cmd), 3); // add r13, r_x
        }

        writeCmd (&curr_pos, (char*) &X86_PUSH_R13, 2);  // push r13
    }

    else
    {
        //
    }

    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(POP, "pop",
{
    if(!CURR_CMD.ram_flag)
    {
        uint32_t tmp_cmd = X86_POP_R_X[CURR_CMD.reg_value - 1];
        writeCmd (&curr_pos, (char*) &(tmp_cmd), 1);
    }

    break;

    //writeCmd (&curr_pos, (char*) &X86_ADD_RSP, 4);
})

//-----------------------------------------------------------------------------

WRITE_CMD(ADD, "add",
{
    // look for div
})

WRITE_CMD(SUB, "sub",
{
    // look for div
})

WRITE_CMD(MUL, "mul",
{
    // look for div
})

WRITE_CMD(DIV, "div",
{
    writeCmd (&curr_pos, (char*) &X86_MOV_XMM0_STK, 6); // pop xmm0
    writeCmd (&curr_pos, (char*) &X86_MOV_XMM1_STK, 6); // pop xmm1

    char *sel_cmd = NULL;

    switch(CURR_CMD.command)
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
    writeCmd (&curr_pos, sel_cmd, 4);                    // op xmm1, xmm0 -> xmm1

    writeCmd (&curr_pos, (char*) &X86_ADD_RSP, 4); 
    writeCmd (&curr_pos, (char*) &X86_MOV_STK_XMM1, 6);  // push xmm1     
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(OUT, "out",
{
    writeCmd (&curr_pos, (char*) &X86_LEA_RDI_RSP, 4);

    writeCmd (&curr_pos, (char*) &X86_PUSH_RBP, 1);
    writeCmd (&curr_pos, (char*) &X86_PUSHA, 6);
    writeCmd (&curr_pos, (char*) &X86_MOV_RBP_RSP, 3);

    writeCmd (&curr_pos, (char*) &X86_CALL, 1);
    uint32_t out_ptr = (uint64_t) double_printf - (uint64_t)(curr_pos + 4); 
    writePtr (&curr_pos, out_ptr);

    writeCmd (&curr_pos, (char*) &X86_MOV_RSP_RBP, 3);
    writeCmd (&curr_pos, (char*) &X86_POPA, 6);
    writeCmd (&curr_pos, (char*) &X86_POP_RBP, 1);

    writeCmd (&curr_pos, (char*) &X86_ADD_RSP, 4);
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(DUMP, "dump", 
{
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(JBE, "jbe",
{
    // look for jmp
})

WRITE_CMD(JAE, "jae",
{
    // look for jmp
})

WRITE_CMD(JA, "ja",
{
    // look for jmp
})

WRITE_CMD(JB, "jb",
{
    // look for jmp
})

WRITE_CMD(JE, "je",
{
    // look for jmp
})

    //int pos_ch = arg_value; //15
    //curr_pos = pos_ch - 1;

WRITE_CMD(JNE, "jne",
{
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(JMP, "jmp",
{

    //////////////////

    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(CALL, "call",
{
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(RET, "ret",
{
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(SQRT, "sqrt",
{
    writeCmd (&curr_pos, (char*) &X86_MOV_XMM0_STK, 6);  // pop  xmm0
    writeCmd (&curr_pos, (char*) &X86_SQRTPD, 4);        // sqrt xmm0
    writeCmd (&curr_pos, (char*) &X86_MOV_STK_XMM0, 6);  // push xmm0  
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(IN, "in",
{
    //
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(SIN, "sin",
{
    //
    break;
})

//-----------------------------------------------------------------------------

WRITE_CMD(COS, "cos",
{
    //
    break;
})

//-----------------------------------------------------------------------------
