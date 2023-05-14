
//{----------------------------------------------------------------------------
//!                       OPERATIONS CONVERSION
//}----------------------------------------------------------------------------

CONVERT(ASG,
{
    convert_to_asm (curr_node->right, info);

    trprint ("pop [%d]\n", (int) curr_node->left->val.var);
})

//-----------------------------------------------------------------------------

CONVERT(OVER,
{
    int label_1 = ++info->curr_label;
    int label_2 = ++info->curr_label;

    convert_to_asm (curr_node->left, info);
    convert_to_asm (curr_node->right, info);

    trprint ("ja %d:\n", label_1);

    trprint ("push 0\n");

    trprint ("jmp %d:\n", label_2);

    trprint (":%d\n", label_1);

    trprint ("push 1\n");

    trprint (":%d\n", label_2);
})

//-----------------------------------------------------------------------------

CONVERT(LESS,
{
    int label_1 = ++info->curr_label;
    int label_2 = ++info->curr_label;

    convert_to_asm (curr_node->left, info);
    convert_to_asm (curr_node->right, info);

    trprint ("jb %d:\n", label_1);

    trprint ("push 0\n");

    trprint ("jmp %d:\n", label_2);

    trprint (":%d\n", label_1);

    trprint ("push 1\n");

    trprint (":%d\n", label_2);
})

//-----------------------------------------------------------------------------

CONVERT(EQL,
{
    int label_1 = ++info->curr_label;
    int label_2 = ++info->curr_label;

    convert_to_asm (curr_node->left, info);
    convert_to_asm (curr_node->right, info);

    trprint ("je %d:\n", label_1);

    trprint ("push 0\n");

    trprint ("jmp %d:\n", label_2);

    trprint (":%d\n", label_1);

    trprint ("push 1\n");

    trprint (":%d\n", label_2);
})

//-----------------------------------------------------------------------------

CONVERT(SCAN,
{
    trprint ("in\n");

    trprint ("pop [%d]\n", (int) curr_node->right->val.var);
})

//-----------------------------------------------------------------------------

CONVERT(IF,
{
    int label_1 = ++info->curr_label;
    int label_2 = ++info->curr_label;

    convert_to_asm (curr_node->left, info);

    trprint ("push 0\n");

    trprint ("je %d:\n", label_1);

    convert_to_asm (curr_node->right->left, info);

    trprint ("jmp %d:\n", label_2);

    trprint (":%d\n", label_1);

    if(curr_node->right->right)
    {
        convert_to_asm (curr_node->right->right, info);
    }

    trprint (":%d\n", label_2);
})

//-----------------------------------------------------------------------------

CONVERT(WHILE,
{
    int label_1 = ++info->curr_label;
    int label_2 = ++info->curr_label;

    trprint (":%d\n", label_1);

    convert_to_asm (curr_node->left, info);

    trprint ("push 0\n");

    trprint ("je %d:\n", label_2);

    convert_to_asm (curr_node->right, info);

    trprint ("jmp %d:\n", label_1);

    trprint (":%d\n", label_2);
})

//-----------------------------------------------------------------------------

CONVERT(FUNCT,
{
    int label_1 = ++info->curr_label;

    int num_of_funct = (int) curr_node->left->val.var;

    trprint ("jmp %d:\n", label_1);

    trprint (":%d\n", num_of_funct);

    convert_to_asm (curr_node->right->right, info);

    trprint ("ret\n");

    trprint (":%d\n", label_1);
})

//-----------------------------------------------------------------------------

CONVERT(CALL,
{
    int num_of_funct = (int) curr_node->left->val.var;

    trprint ("call %d:\n", num_of_funct);
})

//-----------------------------------------------------------------------------
