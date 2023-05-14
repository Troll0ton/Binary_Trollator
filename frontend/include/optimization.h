//! @file frontend.h

#ifndef OPTIMIZATION
#define OPTIMIZATION

//-----------------------------------------------------------------------------

#include "frontend/include/tree.h"
#include <math.h>

//-----------------------------------------------------------------------------

bool  simplify_tree   (Node *curr_node, Tree_info *info);

bool  simplify_node   (Node *curr_node, Tree_info *info);

bool  simplify_const  (Node *curr_node, Tree_info *info);

//-----------------------------------------------------------------------------

#endif //OPTIMIZATION
