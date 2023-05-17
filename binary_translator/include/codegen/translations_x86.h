
CMD_EMIT(HLT, "hlt",
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(PUSH, "push",
{
    emitCmd (&curr_pos, (char*) &X86_MOV_R13, 2);        // mov r13, imm64
    emitNum (&curr_pos, (long long int) num); 

    emitCmd (&curr_pos, (char*) &X86_PUSH_R13, 2); // push r13
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(POP, "pop",
{
    emitCmd (&curr_pos, (char*) &X86_POP_RBX, 1); 
    break;
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

    emitCmd (&curr_pos, (char*) &X86_MOV_STK_XMM1, 6);  // push xmm1
    emitCmd (&curr_pos, (char*) &X86_ADD_RSP, 4);        
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(OUT, "out",
{
    emitCmd (&curr_pos, (char*) &X86_PUSHA, 6);

    emitCmd (&curr_pos, (char*) &X86_CALL, 1);
    uint32_t out_ptr = (uint64_t) printLongLongInt - (uint64_t)(curr_pos + 5);                              
    emitPtr (&curr_pos, out_ptr);

    emitCmd (&curr_pos, (char*) &X86_POPA, 6);
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
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(JAE, "jae",
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(JA, "ja",
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(JB, "jb",
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(JE, "je",
{
    break; 
})

//-----------------------------------------------------------------------------

CMD_EMIT(JNE, "jne",
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(JMP, "jmp",
{
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
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(IN, "in",
{
    break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(SIN, "sin",
{
   break;
})

//-----------------------------------------------------------------------------

CMD_EMIT(COS, "cos",
{
    break;
})

//-----------------------------------------------------------------------------
