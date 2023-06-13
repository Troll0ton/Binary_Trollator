//! @file common.h <- Binary translator

#ifndef BT_COMMON_H
#define BT_COMMON_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include <sys/mman.h>

//-----------------------------------------------------------------------------

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

//-----------------------------------------------------------------------------

#define N(suffix) NUM_OF_##suffix
#define O(suffix) OFFSET_##suffix
#define S(suffix) SIZE_OF_##suffix

//-----------------------------------------------------------------------------

static const uint32_t ir_reg_mask = 1;

//-----------------------------------------------------------------------------

typedef double   signature;
typedef double   elem_t;
typedef uint32_t cmd_code;

//-----------------------------------------------------------------------------

#define IS_CONDITION_JUMP(i)                               \
    ((i == JE) || (i == JNE) || (i == JBE) || (i == JB) || \
     (i == JA) || (i == JAE))

#define IS_JUMP(i)                       \
    (IS_CONDITION_JUMP(i) || (i == JMP))

//-----------------------------------------------------------------------------

enum CMD_CODES
{
#define CMD_DEF(cmd, ...) \
    cmd,

#include "processor/COMMON/include/codegen/codegen.h"

#undef CMD_DEF
};

//-----------------------------------------------------------------------------

typedef struct IR_node
{
    cmd_code command;
    int imm_value;
    int reg_value;
    char ram_flag;

    int troll_pos;
    char *x64_pos;
} IR_node;

typedef struct IR
{
    IR_node *buffer;
    int size;
} IR;

//-----------------------------------------------------------------------------

#endif // BT_COMMON_H