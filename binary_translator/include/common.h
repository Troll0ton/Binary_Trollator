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

const uint32_t X86_MOV_RAX_IMM = 0xB8;
const uint32_t X86_PUSH_RAX = 0x50;
const uint32_t X86_POP_RBX = 0x59;
const uint32_t X86_CALL = 0xE8;
const uint32_t X86_RET = 0xC3;
const uint32_t X86_MOV_R10 = 0xBA49;

const uint64_t X86_PUSHA = 0x505152535241;
const uint64_t X86_POPA = 0x5A415B5A5958;  

//-----------------------------------------------------------------------------

typedef struct Ir_code
{
    cmd_code command;
    int  imm_value;
    int  reg_value;
    char ram_flag;
} Ir_code;

typedef struct Intrm_represent
{
    Ir_code *buffer;
    int size;
} Intrm_represent;

typedef struct X86_represent
{
    char *code;
    int size;
} X86_represent;


//-----------------------------------------------------------------------------

#endif //BT_COMMON_H