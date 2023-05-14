//! @file frontend.h

#ifndef FRONTEND_H
#define FRONTEND_H

//-----------------------------------------------------------------------------

#include "frontend/include/tree.h"

//-----------------------------------------------------------------------------

#define O(suffix) OFFSET_##suffix

//-----------------------------------------------------------------------------

enum OFFSETS
{
    OFFSET_ASG   = 1,    //assignment
    OFFSET_IF    = 2,    //condition
    OFFSET_ELSE  = 4,    //else
    OFFSET_FUNCT = 5,    //function initializing
    OFFSET_WHILE = 5,    //while
    OFFSET_NAME  = 2,    //define name
    OFFSET_OUT   = 5,    //out
    OFFSET_SCAN  = 4     //scanning
};

//-----------------------------------------------------------------------------

Node *get_grammar    (char **grammar, Tree_info *tree_info, Text_info *text_info);

Node *get_body       (char **grammar, Tree_info *tree_info, Text_info *text_info);

Node *get_expression (char **grammar);

Node *get_mul_div    (char **grammar);

Node *get_brackets   (char **grammar);

Node *get_str        (char **grammar);

Node *get_syllable   (char **grammar);

Node *get_pow        (char **grammar);

Node *get_math_funct (char **grammar, char *name);

Node *get_assignment (char **grammar);

Node *get_statement  (Tree_info *tree_info, Text_info *text_info);

//-----------------------------------------------------------------------------

#endif //FRONTEND_H
