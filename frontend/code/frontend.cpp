#include "frontend/include/frontend.h"

//{----------------------------------------------------------------------------
//!                                (and with union value)
//! INIT_OP_1 - initialize operator-node with 1-priority and left and right son;
//! INIT_OP_2 - initialize operator-node with 2-priority and left and right son;
//! INIT_OP_3 - initialize operator-node with 3-priority and left and right son;
//! INIT_OP_4 - initialize operator-node with 4-priority;
//!
//! 1 = +, -
//! 2 = *, /
//! 3 = ^
//! 4 = sin, cos, tg, ctg, ln, arcsin, arccos, arctg, arcctg
//!
//}----------------------------------------------------------------------------

#define CURR_LINE text_info->Text[text_info->curr_line].begin_line

Node *get_mul_div (char **grammar)
{
    Node *left_node = get_pow (grammar);

    while(IS_('*') || IS_('/'))
    {
        char curr_op = **grammar;
        (*grammar)++;

        Node *new_node = NULL;

        value val = { 0 };

        Node *right_node = get_pow (grammar);

        switch(curr_op)
        {
            case '*':
            {
                // filled_node oper_name oper_priority
                //       V         V           V
                INIT (new_node,   MUL,         2);
                break;
            }
            case '/':
            {
                INIT (new_node, DIV, 2);
                break;
            }
            default:
            {
                printf ("UNKOWN COMMAND - PRIORITY 2!\n");
                break;
            }
        }

        left_node = new_node;
    }

    return left_node;
}

//-----------------------------------------------------------------------------

Node *get_pow (char **grammar)
{
    Node *left_node = get_brackets (grammar);

    while(IS_('^'))
    {
        (*grammar)++;

        Node *new_node = NULL;

        value val = { 0 };

        Node *right_node = get_brackets (grammar);

        INIT (new_node, POW, 3);

        left_node = new_node;
    }

    return left_node;
}

//-----------------------------------------------------------------------------

Node *get_brackets (char **grammar)
{
    Node *new_node = NULL;

    if(IS_('('))
    {
        (*grammar)++;

        new_node = get_expression (grammar);

        CHECK_EXPRESSION (**grammar == ')', "CLOSING BRACKET\n");

        (*grammar)++;
    }

    else
    {
        new_node = get_syllable (grammar);
    }

    return new_node;
}

//-----------------------------------------------------------------------------

Node *get_expression (char **grammar)
{
    Node *left_node = get_mul_div (grammar);

    while(IS_('+') || IS_('-'))
    {
        char curr_op = **grammar;
        (*grammar)++;

        Node *new_node = NULL;

        value val = { 0 };

        Node *right_node = get_mul_div (grammar);

        switch(curr_op)
        {
            case '+':
            {
                INIT (new_node, ADD, 1);
                break;
            }
            case '-':
            {
                INIT (new_node, SUB, 1);
                break;
            }
            default:
            {
                printf ("UNKOWN COMMAND - PRIORITY 1\n");
                break;
            }
        }

        left_node = new_node;
    }

    return left_node;
}

//-----------------------------------------------------------------------------

Node *get_grammar (char **grammar, Tree_info *tree_info, Text_info *text_info)
{
    if(**grammar == '\0' || **grammar == '\r' || **grammar == '\n') //handle empty lines
    {
        text_info->curr_line++;

        return get_grammar (&CURR_LINE, tree_info, text_info);
    }

    Node *comparison_node   = NULL; //comparison (like x>20 for example, in IF or WHILE grammar)
    char *end_of_comparison = NULL;

    ///HANDLE SPECIAL FUNCTIONS AND OPERATIONS
    // (IF, WHILE, COMPARISON, FUNCTION'S INIT and others)

    #define HANDLE_OP(CONDITION, ...)    \
    if(CONDITION)                        \
    {                                    \
        __VA_ARGS__                      \
    }

    //-----------------------------------------------------------------------------

    #include "frontend/include/codegen/get_op.h"

    //-----------------------------------------------------------------------------

    #undef HANDLE_OP

    ///HANDLE GRAMMAR
    Node *root = get_expression (grammar);

    CHECK_EXPRESSION (**grammar == '\0', "END OF EXPRESSION: line %d\n", text_info->curr_line);

    return root;
}

//-----------------------------------------------------------------------------

Node *get_body (char **grammar, Tree_info *tree_info, Text_info *text_info)
{
    if(**grammar == '{')
    {
        text_info->curr_line++;

        return get_statement (tree_info, text_info);
    }

    (*grammar)++;

    return get_grammar (grammar, tree_info, text_info);
}

//-----------------------------------------------------------------------------

Node *get_statement (Tree_info *tree_info, Text_info *text_info)
{
    Node *new_node = NULL;

    value val = { 0 };

    Node *left_node = get_grammar (&CURR_LINE, tree_info, text_info);

    nullify_tree_pars (tree_info);

    text_info->curr_line++;

    while(text_info->curr_line < text_info->File_input->num_of_lines && CURR_LINE[0] != '}')
    {
        Node *right_node = get_grammar (&CURR_LINE, tree_info, text_info);

        text_info->curr_line++;

        INIT (new_node, ST, 0);

        left_node = new_node;
    }

    return left_node;
}

//-----------------------------------------------------------------------------

Node *get_str (char **grammar)
{
    Node *new_node = NULL;

    char name[MAX_LEN] = "";

    value val = { 0 };

    int pos = 0;

    while(isalpha (**grammar) && pos < MAX_LEN)
    {
        name[pos] = **grammar;
        pos++;

        (*grammar)++;
    }

    if(IS_('('))
    {
        return get_math_funct (grammar, name);
    }

    val.var = *name;

    INIT_VAR (val);

    return new_node;
}

//-----------------------------------------------------------------------------

Node *get_math_funct (char **grammar, char *name)
{
    Node *new_node   = NULL;
    Node *left_node  = NULL;
    Node *right_node = NULL;

    value val = { 0 };

    #define OP_DEF(op, op_name, asm_name, code, ...)    \
    if(stricmp (name, op_name) == 0)                    \
    {                                                   \
        right_node = get_brackets (grammar);            \
        INIT (new_node, op, 4);                         \
                                                        \
        return new_node;                                \
    }

    //-----------------------------------------------------------------------------

    #include "common/include/codegen/op_def.h"

    //-----------------------------------------------------------------------------

    #undef OP_DEF

    value val_funct = { 0 };

    val_funct.var = *name;

    (*grammar) += O(NAME);

    left_node = create_node ();

    ASSIGN_NODE (left_node, NULL, NULL, NULL, VAR, val_funct, 4);

    //right_node = get_brackets (grammar);  (PARAMS INITIALIZING)  -  NOT COMPLETED!!!

    INIT (new_node, CALL, 0);

    return new_node;
}

//-----------------------------------------------------------------------------

#define VALUE_NUM val.num

Node *get_syllable (char **grammar)
{
    Node *new_node = NULL;

    value val = { 0 };

    const char *str_old = *grammar;

    if(isalpha (**grammar))
    {
        return get_str (grammar);
    }

    if(is_num (**grammar))
    {
        int num_offset = 0;

        sscanf (*grammar, "%lf%n", &VALUE_NUM, &num_offset);

        *grammar += num_offset;
    }

    INIT_NUM (val);

    CHECK_EXPRESSION (*grammar != str_old, "ERROR WHILE GET NUM\n");

    return new_node;
}

#undef VALUE_NUM
#undef CURR_LINE

//-----------------------------------------------------------------------------



