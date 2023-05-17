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

const int absence = 0xBEDBAD;

//-----------------------------------------------------------------------------

enum CMD_CODES
{
    #define CMD_DEF(cmd, ...) \
    cmd,

    #include "processor/COMMON/include/codegen/codegen.h"

    #undef CMD_DEF
};

//-----------------------------------------------------------------------------

const uint32_t MASK_PUSH_REG = 0x50;

//-----------------------------------------------------------------------------
//                           
//-----------------------------------------------------------------------------

const uint32_t X86_CALL = 0xE8;
const uint32_t X86_RET = 0xC3;

const uint32_t X86_MOV_R10 = 0xBA49;
const uint32_t X86_MOV_R13 = 0xBD49;

//-----------------------------------------------------------------------------
//                              STACK CODES
//-----------------------------------------------------------------------------

const uint32_t X86_PUSH_RAX = 0x50;
const uint32_t X86_PUSH_R13 = 0x5541;

const uint32_t X86_POP_RBX = 0x59;

const uint64_t X86_PUSHA = 0x505152535241;
const uint64_t X86_POPA = 0x5A415B5A5958; 
                             
const uint32_t X86_ADD_RSP = 0x08C48348; // add rsp, 8

const uint64_t X86_MOV_XMM0_STK = 0x082444100FF2; // movsd xmm0, [rsp+8]
const uint64_t X86_MOV_XMM1_STK = 0x10244C100FF2; // movsd xmm1, [rsp+16]
const uint64_t X86_MOV_STK_XMM1 = 0x10244CD60F66; // movq [rsp+16], xmm1

//-----------------------------------------------------------------------------
//                               SSE CODES
//-----------------------------------------------------------------------------

const uint32_t X86_ADDSD = 0xC8580FF2; // addsd xmm1, xmm0
const uint32_t X86_SUBSD = 0xC85C0FF2; // subsd xmm1, xmm0
const uint32_t X86_MULSD = 0xC8590FF2; // mulsd xmm1, xmm0
const uint32_t X86_DIVSD = 0xC85E0FF2; // divsd xmm1, xmm0

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