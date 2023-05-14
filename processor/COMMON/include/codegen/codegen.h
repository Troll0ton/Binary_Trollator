
CMD_DEF(HLT, "hlt",
{
    cpu->is_stop = true;
})

CMD_DEF(PUSH, "push",
{
    stack_push (&cpu->Stk, arg_value);
})

CMD_DEF(POP, "pop",
{
    *curr_arg = stack_pop (&cpu->Stk);
})

CMD_DEF(ADD, "add",
{
    stack_push (&cpu->Stk, stack_pop (&cpu->Stk) + stack_pop (&cpu->Stk));
})

CMD_DEF(SUB, "sub",
{
    stack_push (&cpu->Stk, -(stack_pop (&cpu->Stk) - stack_pop (&cpu->Stk)));
})

CMD_DEF(MUL, "mul",
{
    stack_push (&cpu->Stk, stack_pop (&cpu->Stk) * stack_pop (&cpu->Stk));
})

CMD_DEF(DIV, "div",
{
    stack_push (&cpu->Stk, 1 / stack_pop (&cpu->Stk) * stack_pop (&cpu->Stk));
})

CMD_DEF(OUT, "out",
{
    double value = stack_pop (&cpu->Stk);

    if(value == POISON_STK)
    {
        printf ("not existed\n");
    }

    else
    {
        printf ("result: %lg\n", value);
    }
})

CMD_DEF(DUMP, "dump",
{
    stack_dumps (&cpu->Stk, cpu->info.file_out);
})

CMD_DEF(JBE, "jbe",
{
    CJMP(first_number <= second_number);
})

CMD_DEF(JAE, "jae",
{
    CJMP(first_number >= second_number);
})

CMD_DEF(JA, "ja",
{
    CJMP(first_number > second_number);
})

CMD_DEF(JB, "jb",
{
    CJMP(first_number < second_number);
})

CMD_DEF(JE, "je",
{
    CJMP(is_equal(first_number, second_number));
})

CMD_DEF(JNE, "jne",
{
    CJMP(!is_equal(first_number, second_number));
})


CMD_DEF(JMP, "jmp",
{
    int pos_ch = arg_value;
    curr_pos = pos_ch - 1;
})

CMD_DEF(CALL, "call",
{
    stack_push (&cpu->Stk_call, ++curr_pos);

    int pos_ch = arg_value;
    curr_pos   = pos_ch - 1;
})

CMD_DEF(RET, "ret",
{
    curr_pos = stack_pop (&cpu->Stk_call) - 1;
})

CMD_DEF(SQRT, "sqrt",
{
    double value = stack_pop (&cpu->Stk);

    if(value >= 0)
    {
        stack_push (&cpu->Stk, sqrt (value));
    }

    else
    {
        printf ("ERROR - sqrt below zero!\n");
    }
})

CMD_DEF(IN, "in",
{
    printf("\nENTER YOUR NUMBER:\n");

    double value = 0;

    scanf ("%lg", &value);

    stack_push (&cpu->Stk, value);
})

CMD_DEF(SIN, "sin",
{
    double value = stack_pop (&cpu->Stk);

    stack_push (&cpu->Stk, sin(value));
})

CMD_DEF(COS, "cos",
{
    double value = stack_pop (&cpu->Stk);

    stack_push (&cpu->Stk, cos(value));
})
