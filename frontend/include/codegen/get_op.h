
//{----------------------------------------------------------------------------
//!                       SPECIAL GRAMMAR PROCESSING
//!                             (IN FRONTEND)
//}----------------------------------------------------------------------------

HANDLE_OP((strchr (*grammar, '>') || strchr (*grammar, '<') || strchr (*grammar, '~')),
{
    Node *new_node = NULL;

    value val = { 0 };

    char *old_pos = *grammar;

    *grammar = strchr (*grammar, '(');
    (*grammar)++;

    Node *left_node = get_brackets (grammar);

    char curr_op = **grammar;
    (*grammar)++;

    Node *right_node = get_brackets (grammar);
    (*grammar)++;

    end_of_comparison = *grammar;

    switch(curr_op)
    {
        case '>':
        {
            INIT (new_node, OVER, 0);
            break;
        }
        case '<':
        {
            INIT (new_node, LESS, 0);
            break;
        }
        case '~':
        {
            INIT (new_node, EQL, 0);
            break;
        }
        default:
        {
            printf ("UNKOWN COMMAND - COMPARISON!\n");
            break;
        }
    }

    comparison_node = new_node;

    *grammar = old_pos;
})

//-----------------------------------------------------------------------------

HANDLE_OP(!strncmp (*grammar, "if", O(IF)),
{
    Node *new_node  = NULL;
    Node *condition = NULL;

    value val = { 0 };

    (*grammar) += O(IF);

    if(comparison_node)
    {
        condition = comparison_node;

        *grammar  = end_of_comparison;
    }

    else
    {
        condition = get_brackets (grammar);
    }

    Node *if_body    = NULL;
    Node *left_node  = get_body (grammar, tree_info, text_info);
    Node *right_node = NULL;

    text_info->curr_line++;

    char **curr_sym = &CURR_LINE;

    if(!strncmp (*curr_sym, "else", O(ELSE)))
    {
        (*curr_sym) += O(ELSE);

        right_node = get_body (curr_sym, tree_info, text_info);
    }

    else
    {
        text_info->curr_line--;

        right_node = NULL;
    }

    INIT (if_body, BODY, 0);

    left_node  = condition;
    right_node = if_body;

    INIT (new_node, IF, 0);

    return new_node;
})

//-----------------------------------------------------------------------------

HANDLE_OP(!strncmp (*grammar, "print", O(OUT)),
{
    Node *new_node   = NULL;
    Node *left_node  = NULL;
    Node *right_node = NULL;

    value val = { 0 };

    (*grammar) += O(OUT);

    right_node = get_brackets (grammar);

    INIT (new_node, OUT, 0);

    return new_node;
})

//-----------------------------------------------------------------------------

HANDLE_OP(!strncmp (*grammar, "scan", O(SCAN)),
{
    Node *new_node   = NULL;
    Node *left_node  = NULL;
    Node *right_node = NULL;

    value val = { 0 };

    (*grammar) += O(SCAN);

    right_node = get_brackets (grammar);

    INIT (new_node, SCAN, 0);

    return new_node;
})

//-----------------------------------------------------------------------------

HANDLE_OP(!strncmp (*grammar, "funct", O(FUNCT)),
{
    Node *new_node = NULL;

    value val = { 0 };

    (*grammar) += O(FUNCT);
    (*grammar)++;

    Node *funct_name = get_str (grammar);

    text_info->curr_line++;

    Node *funct_body = NULL;
    Node *left_node  = NULL;

    Node *right_node = get_statement (tree_info, text_info);

    INIT (funct_body, BODY, 0);

    left_node  = funct_name;
    right_node = funct_body;

    INIT (new_node, FUNCT, 0);

    return new_node;
})

//-----------------------------------------------------------------------------

HANDLE_OP(!strncmp (*grammar, "while", O(WHILE)),
{
    Node *new_node  = NULL;
    Node *left_node = NULL;

    value val = { 0 };

    (*grammar) += O(WHILE);

    if(comparison_node)
    {
        left_node = comparison_node;

        *grammar = end_of_comparison;
    }

    else
    {
        left_node = get_brackets (grammar);
    }

    Node *right_node = get_body (grammar, tree_info, text_info);

    INIT (new_node, WHILE, 0);

    return new_node;
})

//-----------------------------------------------------------------------------

HANDLE_OP(strchr (*grammar, '='),
{
    Node *new_node = NULL;

    value val = { 0 };

    Node *left_node = get_str (grammar);

    (*grammar) += O(ASG);

    Node *right_node = get_expression (grammar);

    CHECK_EXPRESSION (**grammar == '\0', "END OF EXPRESSION\n");

    INIT (new_node, ASG, 0);

    return new_node;
})

//-----------------------------------------------------------------------------
