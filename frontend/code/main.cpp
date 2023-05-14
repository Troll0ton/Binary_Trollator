#include "frontend/include/frontend.h"
#include "frontend/include/optimization.h"

//-----------------------------------------------------------------------------

int main ()
{
    Text_info text_info = { 0 };
    text_info_ctor (&text_info);

    Tree_info tree_info = { 0 };
    tree_info_ctor (&tree_info);

    tree_info.root = get_statement (&tree_info, &text_info);

    simplify_tree (tree_info.root, &tree_info);

    save_tree (tree_info.root, &tree_info);

    tree_dump (&tree_info);

    tree_dtor (tree_info.root);

    text_info_dtor (&text_info);
    tree_info_dtor (&tree_info);

    return 0;
}

//-----------------------------------------------------------------------------
