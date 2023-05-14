//! @file stack.h

#ifndef   STACK_H
#define   STACK_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <locale.h>
#include <limits.h>
#include <stdint.h>

//-----------------------------------------------------------------------------

#define LEFT_CANARY  0xBADDED
#define RIGHT_CANARY 0xDEDBAD

//-----------------------------------------------------------------------------

#define NUM_OF_MIS 8

//-----------------------------------------------------------------------------

#define ERROR_FIELD 0

//-----------------------------------------------------------------------------

#define stack_ctor(stk, capacity) stack_ctor_ (stk, capacity, #stk, __FILE__,  __LINE__)
#define stack_dump(stk)           stack_dump_ (stk,                 __FILE__,  __LINE__)

//-----------------------------------------------------------------------------

#define stk_increase 1
#define stk_decrease 0

//-----------------------------------------------------------------------------

typedef struct StackInfo   //////////////////////
{
    const char *name;
    const char *file;
    int         line;
    int32_t     error_codes;
    char       *cur_status;
} StackInfo;

//-----------------------------------------------------------------------------

typedef struct Stack       ////////////////////////
{
    int32_t   canary_left;
    StackInfo Stack_info;
    double   *buffer_stk;
    int       capacity_stk;
    int       size_stk;
    int32_t   hash;
    int32_t   canary_right;
} Stack;

//-----------------------------------------------------------------------------

typedef struct Error_info
{
    int32_t error_code;
    char   *error_output;
} Error_info;

//-----------------------------------------------------------------------------

enum ERROR_CODES
{
    ERR_CAN_1  = (1 << 1),
    ERR_CAN_2  = (1 << 2),
    ERR_MEMBUF = (1 << 3),
    ERR_OVERF  = (1 << 4),
    ERR_CAP    = (1 << 5),
    ERR_SIZE   = (1 << 6),
    ERR_MEMSTK = (1 << 7),
    ERR_HASH   = (1 << 8)
};

//-----------------------------------------------------------------------------

const struct Error_info error_arr[]
{
    {ERR_CAN_1,  "ERROR - the integrity of first canary is broken                    \n"},
    {ERR_CAN_2,  "ERROR - the integrity of second canary is broken                   \n"},
    {ERR_MEMBUF, "ERROR - incorrect memory allocation for stack's data (NULL-pointer)\n"},
    {ERR_OVERF,  "ERROR - stack overflow                                             \n"},
    {ERR_CAP,    "ERROR - incorrect capacity value (below zero)                      \n"},
    {ERR_SIZE,   "ERROR - incorrect size value (below zero)                          \n"},
    {ERR_MEMSTK, "ERROR - incorrect memory allocation for stack (NULL-pointer)       \n"},
    {ERR_HASH,   "ERROR - data integrity breach (invalid hash)                       \n"},
};

//-----------------------------------------------------------------------------

int     stack_ctor_      (struct Stack *stk,     int capacity_ctor, const char* stk_name,
                          const char* file_name, int stk_line                            );
void    stack_dtor       (Stack *stk);
double  stack_pop        (struct Stack *stk);
void    stack_push       (struct Stack *stk, double elem);
void    stack_resize     (struct Stack *stk, int opt_resize);
void    stack_dump_      (struct Stack *stk);
void    stack_dumps      (struct Stack *stk, FILE *log_file);
void    verificate_stack (struct Stack *stk, int32_t new_hash);
void    handle_errors    (struct Stack *stk);
void    debug_stack      (struct Stack *stk);
void    check_errors     (struct Stack *stk, int32_t new_hash);
int32_t calculate_hash   (struct Stack *stk);
void   *recalloc         (void *buffer, int capacity, int size, int size_of_type);

//-----------------------------------------------------------------------------

#endif //STACK_H
