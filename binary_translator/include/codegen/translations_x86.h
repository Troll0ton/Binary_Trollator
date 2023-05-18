
CMD_EMIT(HLT, "hlt",
{
    emitCmd (&curr_pos, (char*) &X86_RET, 1);
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(PUSH, "push",
{
    if(!CURR_CMD.ram_flag)
    {
        emitCmd (&curr_pos, (char*) &X86_MOV_R13, 2);  // mov r13, imm64
        emitNum (&curr_pos, (double) num); 

        if(CURR_CMD.reg_value)
        {
            uint32_t tmp_cmd = MASK_X86_ADD_R13_R_X | 
                              (X86_ADD_R13_R_X[CURR_CMD.reg_value - 1] << X86_ADD_R13_R_X_OFFSET);

            emitCmd (&curr_pos, (char*) &(tmp_cmd), 3); // add r13, r_x
        }

        emitCmd (&curr_pos, (char*) &X86_PUSH_R13, 2);  // push r13
    }

    else
    {
        //
    }

    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(POP, "pop",
{
    if(!CURR_CMD.ram_flag)
    {
        uint32_t tmp_cmd = X86_POP_R_X[CURR_CMD.reg_value - 1];
        emitCmd (&curr_pos, (char*) &(tmp_cmd), 1);
    }

    break;

    //emitCmd (&curr_pos, (char*) &X86_ADD_RSP, 4);
})

//-----------------------------------------------------------------------------

CMD_EMIT(ADD, "add",
{
    // look for div
})

CMD_EMIT(SUB, "sub",
{
    // look for div
})

CMD_EMIT(MUL, "mul",
{
    // look for div
})

CMD_EMIT(DIV, "div",
{
    emitCmd (&curr_pos, (char*) &X86_MOV_XMM0_STK, 6); // pop xmm0
    emitCmd (&curr_pos, (char*) &X86_MOV_XMM1_STK, 6); // pop xmm1

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
    emitCmd (&curr_pos, sel_cmd, 4);                    // op xmm1, xmm0 -> xmm1

    emitCmd (&curr_pos, (char*) &X86_ADD_RSP, 4); 
    emitCmd (&curr_pos, (char*) &X86_MOV_STK_XMM1, 6);  // push xmm1     
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(OUT, "out",
{
    emitCmd (&curr_pos, (char*) &X86_LEA_RDI_RSP, 4);

    emitCmd (&curr_pos, (char*) &X86_PUSH_RBP, 1);
    emitCmd (&curr_pos, (char*) &X86_PUSHA, 6);
    emitCmd (&curr_pos, (char*) &X86_MOV_RBP_RSP, 3);

    emitCmd (&curr_pos, (char*) &X86_CALL, 1);
    uint32_t out_ptr = (uint64_t) double_printf - (uint64_t)(curr_pos + 4); 
    emitPtr (&curr_pos, out_ptr);

    emitCmd (&curr_pos, (char*) &X86_MOV_RSP_RBP, 3);
    emitCmd (&curr_pos, (char*) &X86_POPA, 6);
    emitCmd (&curr_pos, (char*) &X86_POP_RBP, 1);

    emitCmd (&curr_pos, (char*) &X86_ADD_RSP, 4);
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(DUMP, "dump", 
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(JBE, "jbe",
{
    // look for jmp
})

CMD_EMIT(JAE, "jae",
{
    // look for jmp
})

CMD_EMIT(JA, "ja",
{
    // look for jmp
})

CMD_EMIT(JB, "jb",
{
    // look for jmp
})

CMD_EMIT(JE, "je",
{
    // look for jmp
})

    //int pos_ch = arg_value; //15
    //curr_pos = pos_ch - 1;

CMD_EMIT(JNE, "jne",
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(JMP, "jmp",
{

    //////////////////

    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(CALL, "call",
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(RET, "ret",
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(SQRT, "sqrt",
{
    emitCmd (&curr_pos, (char*) &X86_MOV_XMM0_STK, 6);  // pop  xmm0
    emitCmd (&curr_pos, (char*) &X86_SQRTPD, 4);        // sqrt xmm0
    emitCmd (&curr_pos, (char*) &X86_MOV_STK_XMM0, 6);  // push xmm0  
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(IN, "in",
{
    //
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(SIN, "sin",
{
    //
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(COS, "cos",
{
    //
    break;
})

//-----------------------------------------------------------------------------
