//! @file common.h <- Binary translator

#ifndef BT_COMMON_H
#define BT_COMMON_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//-----------------------------------------------------------------------------

typedef double signature;
typedef double elem_t;
typedef unsigned int cmd_code;

//-----------------------------------------------------------------------------

enum CMD_CODES
{
    #define CMD_DEF(cmd, ...) \
    cmd,

    #include "processor/COMMON/include/codegen/codegen.h"

    #undef CMD_DEF
};

//-----------------------------------------------------------------------------

typedef struct Ir_code
{
    cmd_code command;
    int  imm_value;
    int  reg_num;
    char ram_flag;
} Ir_code;

typedef struct Intrm_represent
{
    Ir_code *buffer;
    int size;
} Intrm_represent;

//-----------------------------------------------------------------------------

#endif //BT_COMMON_H