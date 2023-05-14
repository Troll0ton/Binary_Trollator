#include "backend/include/backend.h"

//-----------------------------------------------------------------------------

int main ()
{
    Text_info text_info = { 0 };
    text_info_ctor (&text_info);

    Tree_info tree_info = { 0 };
    tree_info_ctor (&tree_info);

    nullify_tree_pars (&tree_info);

    read_tree (&tree_info, &text_info);

    tree_dump (&tree_info);

    create_asm_file (&tree_info);

    tree_dtor (tree_info.root);

    text_info_dtor (&text_info);
    tree_info_dtor (&tree_info);

    return 0;
}

//-----------------------------------------------------------------------------
