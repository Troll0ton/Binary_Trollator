//! @file backend.h

#ifndef BACK_END
#define BACK_END

//-----------------------------------------------------------------------------

#include "backend/include/tree.h"
#include <math.h>

//-----------------------------------------------------------------------------

#define OFFSET_TYPE 1

//-----------------------------------------------------------------------------

#define MAIN 97

//-----------------------------------------------------------------------------

void  create_asm_file    (Tree_info *info);

Node *read_tree          (Tree_info *tree_info, Text_info *text_info);

Node *init_node          (Text_info *text_info);

Node *handle_branch_node (Tree_info *tree_info, Text_info *text_info, Node *new_node);

Node *handle_end_node    (Tree_info *tree_info, Text_info *text_info, Node *new_node);

void  convert_to_asm     (Node *curr_node, Tree_info *info);

void  convert_if         (Node *curr_node, Tree_info *info);

void  convert_funct_def  (Node *curr_node, Tree_info *info);

void  convert_call       (Node *curr_node, Tree_info *info);

void  convert_while      (Node *curr_node, Tree_info *info);

void  convert_assignment (Node *curr_node, Tree_info *info);

void  print_node         (Node *curr_node, Tree_info *info);

//-----------------------------------------------------------------------------

#endif //BACK_END

