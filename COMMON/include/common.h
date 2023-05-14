//! @file common.h

#ifndef COMMON_H
#define COMMON_H

//-----------------------------------------------------------------------------

enum TYPES
{
    VAR,
    NUM,
    OP,
    NUL,
};

//-----------------------------------------------------------------------------

#define OP_DEF(op, op_name, code, ...)  \
    op,

enum OPERATIONS
{
    //-----------------------------------------------------------------------------

    #include "COMMON/include/codegen/op_def.h"

    //-----------------------------------------------------------------------------
};

#undef OP_DEF

//{----------------------------------------------------------------------------
//!                        SEARCHING THE TYPE
//}----------------------------------------------------------------------------

#define IS_(SYM)    \
   **grammar == SYM

//-----------------------------------------------------------------------------

#define IS_TYPE(curr_node, TYP)   \
    curr_node->type == TYP

//-----------------------------------------------------------------------------

#define IS_NULL(curr_node)                                       \
    (curr_node->type == NUM && is_equal (curr_node->val.num, 0))

#define IS_ONE(curr_node)                                        \
    (curr_node->type == NUM && is_equal (curr_node->val.num, 1))

//-----------------------------------------------------------------------------

#define IS_OP(curr_node, TYP)                                       \
    (curr_node->type == OP && curr_node->val.op == TYP)

//-----------------------------------------------------------------------------

#define IS_FUNCT(curr_node)        \
    curr_node->type == OP    &&    \
    curr_node->priority == 4 &&    \
    curr_node->right->type == NUM  \

//-----------------------------------------------------------------------------

#define CASE(SYM, VAL)  \
    case SYM:           \
    {                   \
        val.op = VAL;   \
                        \
        break;          \
    }


//{----------------------------------------------------------------------------
//!                         INITIALIZING DSL
//}----------------------------------------------------------------------------

#define INIT(NEW_NODE, OPERATION, PRIORITY)  \
    val.op = OPERATION;                      \
    INIT_OP(NEW_NODE, val, PRIORITY);

#define INIT_OP(NEW_NODE, VAL, HIGHT) INIT_NODE (NEW_NODE, left_node, right_node, NULL, OP, VAL, HIGHT)

#define INIT_VAR(VAL) INIT_NODE (new_node, NULL, NULL, NULL, VAR, VAL, 4)

#define INIT_NUM(VAL) INIT_NODE (new_node, NULL, NULL, NULL, NUM, VAL, 4)

#define INIT_NODE(new_node, new_left, new_right, new_parent, new_type, new_value, new_priority)                                                                                       \
    new_node = create_node ();                                                                    \
    ASSIGN_NODE(new_node, new_left, new_right, new_parent, new_type, new_value, new_priority)

//-----------------------------------------------------------------------------

#define ASSIGN_NUM(node, new_value)                                                               \
    value val = { 0 };                                                                            \
    val.num = new_value;                                                                          \
    ASSIGN_NODE(node, node->left, node->right, node->parent, NUM, val, 4)                         \

#define ASSIGN_RIGHT(curr_node)                                                                   \
    ASSIGN_NODE (curr_node, old_right->left, old_right->right, curr_node->parent, old_right->type, old_right->val, old_right->priority);

#define ASSIGN_LEFT(curr_node)                                                                    \
    ASSIGN_NODE (curr_node, old_left->left, old_left->right, curr_node->parent, old_left->type, old_left->val, old_left->priority);

//-----------------------------------------------------------------------------

#define ASSIGN_NODE(new_node, new_left, new_right, new_parent, new_type, new_value, new_priority) \
    new_node->left = new_left;                                                                    \
    new_node->right = new_right;                                                                  \
    new_node->parent = new_parent;                                                                \
    new_node->type = new_type;                                                                    \
    new_node->priority = new_priority;                                                            \
                                                                                                  \
    new_node->val = new_value;                                                                    \
                                                                                                  \
    if(new_node->left)                                                                            \
    {                                                                                             \
        new_node->left->parent = new_node;                                                        \
    }                                                                                             \
                                                                                                  \
    if(new_node->right)                                                                           \
    {                                                                                             \
        new_node->right->parent = new_node;                                                       \
    }

//{----------------------------------------------------------------------------
//!                       END OF INITIALIZING
//}----------------------------------------------------------------------------

#define LINES fprintf (info->file_tex, "-------------------------------\\\\\n");

#define ENTER fprintf (info->file_tex, "\\\\\n");

//-----------------------------------------------------------------------------

#define CHECK_EXPRESSION(EXPR, ...)            \
    if(!(EXPR))                                \
    {                                          \
        printf ("DETECTED MISTAKE,");          \
                                               \
        printf (__VA_ARGS__);                  \
                                               \
        return NULL;                           \
    }

//----------------------------------------------------------------------------

#endif //COMMON_H
