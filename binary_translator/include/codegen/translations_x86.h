
CMD_EMIT(HLT, "hlt",
{
    
})

CMD_EMIT(PUSH, "push",
{
    emitCmd (&curr_pos, (char*) &X86_PUSH_RAX, 1);
})

CMD_EMIT(POP, "pop",
{
    emitCmd (&curr_pos, (char*) &X86_POP_RBX, 1); 
})

CMD_EMIT(ADD, "add",
{
    
})

CMD_EMIT(SUB, "sub",
{
    
})

CMD_EMIT(MUL, "mul",
{
   
})

CMD_EMIT(DIV, "div",
{
    
})

CMD_EMIT(OUT, "out",
{
    emitCmd (&curr_pos, (char*) &X86_PUSHA, 6);

    emitCmd (&curr_pos, (char*) &X86_CALL, 1);
    uint32_t out_ptr = (uint64_t) printLongLongInt - (uint64_t)(curr_pos + 5);                              
    emitPtr (&curr_pos, out_ptr);

    emitCmd (&curr_pos, (char*) &X86_POPA, 6);

})

CMD_EMIT(DUMP, "dump", 
{
  
})

CMD_EMIT(JBE, "jbe",
{
  
})

CMD_EMIT(JAE, "jae",
{
  
})

CMD_EMIT(JA, "ja",
{
    
})

CMD_EMIT(JB, "jb",
{
    
})

CMD_EMIT(JE, "je",
{
    
})

CMD_EMIT(JNE, "jne",
{
    
})


CMD_EMIT(JMP, "jmp",
{
    
})

CMD_EMIT(CALL, "call",
{
    
})

CMD_EMIT(RET, "ret",
{
    
})

CMD_EMIT(SQRT, "sqrt",
{
    
})

CMD_EMIT(IN, "in",
{
    
})

CMD_EMIT(SIN, "sin",
{
   
})

CMD_EMIT(COS, "cos",
{
    
})
