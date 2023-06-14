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

#define N(suffix) NUM_OF_##suffix
#define O(suffix) OFFSET_##suffix
#define S(suffix) SIZE_OF_##suffix

//-----------------------------------------------------------------------------

typedef double   signature;
typedef double   elem_t;
typedef uint32_t cmd_code;

//-----------------------------------------------------------------------------

#define CONDITIONAL_JMP_CASE  \
    case JBE:                 \
    case JAE:                 \
    case JA:                  \
    case JB:                  \
    case JE:                  \
    case JNE:  

//-----------------------------------------------------------------------------  

#define COMMON_JMP_CASE       \
    CONDITIONAL_JMP_CASE      \
    case JMP:                 \
    case CALL:                 

//-----------------------------------------------------------------------------

enum CMD_CODES
{
#define CMD_DEF(cmd, ...) \
    cmd,

#include "processor/COMMON/include/codegen/codegen.h"

#undef CMD_DEF
};

//-----------------------------------------------------------------------------

enum TRANSLATOR_INFO
{
    ABSENCE        = 0xBEDBAD,
    DELETED        = 0xDEDAC,
    MPROTECT_ERROR = -1,
};

//-----------------------------------------------------------------------------

enum IR_REGS
{
    IR_RAX = 1,
    IR_RBX = 2,
    IR_RCX = 3,
    IR_RDX = 4,
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