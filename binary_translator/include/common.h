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

static const int absence = 0xBEDBAD;
static const int deleted = 0xDEDAC;
static const int mprotect_error = -1;

//-----------------------------------------------------------------------------

typedef double signature;
typedef double elem_t;
typedef uint32_t cmd_code;

//-----------------------------------------------------------------------------

enum CMD_CODES
{
    #define CMD_DEF(cmd, ...) \
    cmd,

    #include "processor/COMMON/include/codegen/codegen.h"

    #undef CMD_DEF
};

//-----------------------------------------------------------------------------

enum X86_INFO
{
    SIZE_OF_PTR = 4,
    SIZE_OF_ABS_PTR = 8,
    SIZE_OF_NUM = 8,
};

//-----------------------------------------------------------------------------

typedef struct Troll_code
{
    char *buffer;
    int   size;
} Troll_code;

//-----------------------------------------------------------------------------

typedef struct IR_node
{
    cmd_code command;
    int  imm_value;
    int  reg_value;
    char ram_flag;
    int troll_pos;
    char *x86_pos;
} IR_node;

typedef struct IR
{
    IR_node *buffer;
    int size;
} IR;

//-----------------------------------------------------------------------------

typedef struct X86_code
{
    char *buffer;
    int size;
} X86_code;

//-----------------------------------------------------------------------------
//                             STACK UTILS CODES
//-----------------------------------------------------------------------------

static const uint32_t X86_PUSH_RAX = 0x50;
static const uint32_t X86_PUSH_R13 = 0x5541;

static const uint64_t X86_PUSHA = 0x505152535241;
static const uint64_t X86_POPA = 0x5A415B5A5958; 

static const uint32_t X86_PUSH_RBP = 0x55;
static const uint32_t X86_POP_RBP = 0x5D;
                             
static const uint32_t X86_ADD_RSP = 0x08C48348; // add rsp, 8
static const uint32_t X86_SUB_RSP = 0x08EC8348; // sub rsp, 8

static const uint32_t X86_MOV_RBP_RSP = 0xE58948; // mov rbp, rsp
static const uint32_t X86_MOV_RSP_RBP = 0xEC8948; // mov rbp, rsp

static const uint64_t X86_MOV_XMM0_STK = 0x002444100FF2; // movsd xmm0, [rsp]
static const uint64_t X86_MOV_XMM1_STK = 0x08244C100FF2; // movsd xmm1, [rsp+8]

static const uint64_t X86_MOV_STK_XMM0 = 0x002444110FF2; // movsd [rsp], xmm0
static const uint64_t X86_MOV_STK_XMM1 = 0x00244C110FF2; // movsd [rsp], xmm1

static const uint32_t X86_LEA_RDI_RSP = 0x243C8D48; // lea rdi , [rsp]
static const uint32_t X86_AND_RSP_FF = 0xF0E48348;

static const uint32_t X86_CALL = 0xE8;
static const uint32_t X86_RET = 0xC3;

// rax, rbx, rcx, rdx
static const uint32_t X86_POP_R_X[] = {0x58, 0x5B, 0x59, 0x5A};

//-----------------------------------------------------------------------------
//                             SSE UTILS CODES
//-----------------------------------------------------------------------------

static const uint32_t X86_ADDSD  = 0xC8580FF2; // addsd xmm1, xmm0
static const uint32_t X86_SUBSD  = 0xC85C0FF2; // subsd xmm1, xmm0
static const uint32_t X86_MULSD  = 0xC8590FF2; // mulsd xmm1, xmm0
static const uint32_t X86_DIVSD  = 0xC85E0FF2; // divsd xmm1, xmm0
static const uint32_t X86_SQRTPD = 0xC0510F66; // sqrtpd xmm0, xmm0

//-----------------------------------------------------------------------------
//                                 MOVES
//-----------------------------------------------------------------------------

static const uint32_t X86_MOV_R10 = 0xBA49;
static const uint32_t X86_MOV_R13 = 0xBD49;
static const uint64_t X86_MOV_RDI_XMM0 = 0xC77E0F4866;

//-----------------------------------------------------------------------------
//                                 MASKS
//-----------------------------------------------------------------------------

static const uint32_t MASK_X86_ADD_R13_R_X = 0xC10149; // add r13, r_x
static const uint32_t MASK_X86_ADD_R13_RAX = 0b1;
static const uint32_t MASK_X86_ADD_R13_RBX = 0b111;
static const uint32_t MASK_X86_ADD_R13_RCX = 0b11;
static const uint32_t MASK_X86_ADD_R13_RDX = 0b101;
static const uint32_t X86_ADD_R13_R_X_OFFSET = 18; 

static const uint32_t X86_ADD_R13_R_X[]  = {MASK_X86_ADD_R13_RAX, 
                                     MASK_X86_ADD_R13_RBX, 
                                     MASK_X86_ADD_R13_RCX, 
                                     MASK_X86_ADD_R13_RDX};

//-----------------------------------------------------------------------------
//                                  JUMPS
//-----------------------------------------------------------------------------

static const uint32_t X86_JNE = 0x850F;
static const uint32_t X86_JE = 0x840F;
static const uint32_t X86_JA = 0x8F0F;
static const uint32_t X86_JAE = 0x8D0F;
static const uint32_t X86_JB = 0x8C0F;
static const uint32_t X86_JBE = 0x8E0F;

//-----------------------------------------------------------------------------

#endif //BT_COMMON_H