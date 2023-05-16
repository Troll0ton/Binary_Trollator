
CMD_DEF(HLT, "hlt",
{
    cpu->is_stop = true; //0
})

CMD_DEF(PUSH, "push",
{
    stack_push (&cpu->Stk, arg_value); //1
})

CMD_DEF(POP, "pop",
{
    *curr_arg = stack_pop (&cpu->Stk); //2
})

CMD_DEF(ADD, "add",
{
    stack_push (&cpu->Stk, stack_pop (&cpu->Stk) + stack_pop (&cpu->Stk)); //3
})

CMD_DEF(SUB, "sub",
{
    stack_push (&cpu->Stk, -(stack_pop (&cpu->Stk) - stack_pop (&cpu->Stk))); //4
})

CMD_DEF(MUL, "mul",
{
    stack_push (&cpu->Stk, stack_pop (&cpu->Stk) * stack_pop (&cpu->Stk)); //5
})

CMD_DEF(DIV, "div",
{
    stack_push (&cpu->Stk, 1 / stack_pop (&cpu->Stk) * stack_pop (&cpu->Stk)); //6
})

CMD_DEF(OUT, "out",
{
    double value = stack_pop (&cpu->Stk); //7

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
    stack_dumps (&cpu->Stk, cpu->info.file_out); //8
})

CMD_DEF(JBE, "jbe",
{
    CJMP(first_number <= second_number); //9
})

CMD_DEF(JAE, "jae",
{
    CJMP(first_number >= second_number); //10
})

CMD_DEF(JA, "ja",
{
    CJMP(first_number > second_number); //11
})

CMD_DEF(JB, "jb",
{
    CJMP(first_number < second_number); //12
})

CMD_DEF(JE, "je",
{
    CJMP(is_equal(first_number, second_number)); //13
})

CMD_DEF(JNE, "jne",
{
    CJMP(!is_equal(first_number, second_number)); //14
})


CMD_DEF(JMP, "jmp",
{
    int pos_ch = arg_value; //15
    curr_pos = pos_ch - 1;
})

CMD_DEF(CALL, "call",
{
    stack_push (&cpu->Stk_call, ++curr_pos); //16

    int pos_ch = arg_value;
    curr_pos   = pos_ch - 1;
})

CMD_DEF(RET, "ret",
{
    curr_pos = stack_pop (&cpu->Stk_call) - 1; //17
})

CMD_DEF(SQRT, "sqrt",
{
    double value = stack_pop (&cpu->Stk); //18

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
    printf("\nENTER YOUR NUMBER:\n"); //19

    double value = 0;

    scanf ("%lg", &value);

    stack_push (&cpu->Stk, value);
})

CMD_DEF(SIN, "sin",
{
    double value = stack_pop (&cpu->Stk); //20

    stack_push (&cpu->Stk, sin(value));
})

CMD_DEF(COS, "cos",
{
    double value = stack_pop (&cpu->Stk); //21

    stack_push (&cpu->Stk, cos(value));
})
