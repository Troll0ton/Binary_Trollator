
//{----------------------------------------------------------------------------
//!                    SIMPLIFY CODEGEN
//}----------------------------------------------------------------------------

//simplify_mul_div_one

COND_DEF(((IS_OP (curr_node, MUL)  ||
           IS_OP (curr_node, DIV)) &&
           IS_ONE (RIGHT_NODE)       ),
{
    ASSIGN_LEFT (curr_node);
})

COND_DEF((IS_OP (curr_node, MUL) &&
          IS_ONE (LEFT_NODE)       ),
{
    ASSIGN_RIGHT (curr_node);
})

//simplify_pow

COND_DEF((IS_OP (curr_node, POW) &&
          IS_ONE (RIGHT_NODE)      ),
{
    ASSIGN_LEFT (curr_node);
})

COND_DEF((IS_OP  (curr_node, POW) &&
          IS_NULL (RIGHT_NODE)      ),
{
    ASSIGN_NUM (curr_node, 1);
})

//simplify_add_sub_null

COND_DEF((IS_OP (curr_node, ADD) &&
          IS_NULL (LEFT_NODE)       ),
{
    ASSIGN_RIGHT (curr_node);
})

COND_DEF(((IS_OP (curr_node, ADD)  ||
           IS_OP (curr_node, SUB)) &&
           IS_NULL (RIGHT_NODE)      ),
{
    ASSIGN_LEFT (curr_node);
})

//simplify_mul_div_null

COND_DEF((IS_OP (curr_node, MUL)  &&
         (IS_NULL (LEFT_NODE) ||
          IS_NULL (RIGHT_NODE)  )   ),
{
    ASSIGN_NUM (curr_node, 0);

    tree_dtor (LEFT_NODE);
    tree_dtor (RIGHT_NODE);
})

COND_DEF((IS_OP (curr_node, DIV) &&
          IS_NULL (LEFT_NODE)      ),
{
    ASSIGN_NUM (curr_node, 0);

    tree_dtor (LEFT_NODE);
    tree_dtor (RIGHT_NODE);
})

