//! @file tree.h

#ifndef TREE_H
#define TREE_H

//-----------------------------------------------------------------------------

#include "frontend/include/lines_handle.h"
#include "frontend/include/input_output.h"
#include "common/include/common.h"

//-----------------------------------------------------------------------------

#define trprint(...) fprintf (info->file_tree, __VA_ARGS__)

#define dot_print(...) fprintf (info->file_dot, __VA_ARGS__)

//-----------------------------------------------------------------------------

#define tree_info_ctor(info) tree_info_ctor_ (info, __FILE__, __LINE__)

//-----------------------------------------------------------------------------

#define N(suffix) num_of_##suffix

//-----------------------------------------------------------------------------

#define DELETED_PAR -1
#define POISON_PTR  NULL

//-----------------------------------------------------------------------------

typedef union value
{
    double num;
    int op;
    char var;
} value;

//-----------------------------------------------------------------------------

typedef struct Node
{
    Node *parent;
    Node *left;
    Node *right;
    char  priority;
    int   type;
    value val;
} Node;

//-----------------------------------------------------------------------------

enum TREE_INFO
{
    MAX_LEN = 100,
};

//-----------------------------------------------------------------------------

enum TREE_OFFSETS
{
    OFFSET_PARENTHESES = 1,
};

//-----------------------------------------------------------------------------

typedef struct Text_info
{
    FILE  *file_in;
    Line  *Text;
    File  *File_input;
    int    curr_line;
} Text_info;

//-----------------------------------------------------------------------------

typedef struct Tree_info
{
    FILE  *file_dump;
    FILE  *file_dot;
    FILE  *file_tree;
    Node  *root;
    Node  *curr_parent;
    double var_value;
    int    graphviz_node;
    int    graph_num;
    const char *name;
    const char *log_file;
    int         line;
} Tree_info;

//-----------------------------------------------------------------------------

enum SIDES
{
    left,
    right,
};

//-----------------------------------------------------------------------------

void  tree_info_ctor_   (Tree_info *info, const char* log_file, int line);

void  text_info_ctor    (Text_info *info);

void  text_info_dtor    (Text_info *info);

void  nullify_tree_pars (Tree_info *info);

void  tree_info_dtor    (Tree_info *info);

Node *create_node       ();

Node *create_root       (int type, value val, Tree_info *info);

void  tree_dump         (Tree_info *info);

void  save_tree         (Node *curr_node, Tree_info *info);

void  create_tree_graph (Tree_info *info);

Node *copy_tree         (Node *orig_root, Tree_info *info);

Node *copy_node         (Node *curr_node, Tree_info *info);

void  print_node        (Node *curr_node, Tree_info *info);

void  create_graphviz_node (Node *root, Tree_info *info);

void  build_connections (Node *root, Tree_info *info);

void  tree_dtor         (Node *curr_node);

//-----------------------------------------------------------------------------

#endif //TREE_H
