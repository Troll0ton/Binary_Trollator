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

enum TRANSLATOR_INFO
{
    absence = 0xBEDBAD,
    deleted = 0xDEDAC,
    mprotect_error = -1,
};

//-----------------------------------------------------------------------------

typedef double signature;
typedef double elem_t;
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

enum X86_INFO
{
    SIZE_OF_PTR = 4,
    SIZE_OF_ABS_PTR = 8,
    SIZE_OF_NUM = 8,

    X86_ADD_R13_R_X_OFFSET = 18,
    X86_CODE_SIZE = PAGESIZE * 2,

    X86_CONDITIONAL_JUMP_OFFSET = 20,
    X86_JUMP_OFFSET = 1,
    X86_CALL_OFFSET = 15,
};

//-----------------------------------------------------------------------------

enum MASK_INFO
{
    JMP_MASK_LEN = 8,
    X86_MOV_R_X_STK_MASK_LEN = 18,
};

//-----------------------------------------------------------------------------

enum MEMORY_INFO
{
    MEMORY_SIZE = PAGESIZE,
};

//-----------------------------------------------------------------------------

typedef struct Troll_code
{
    char *buffer;
    int size;
} Troll_code;

//-----------------------------------------------------------------------------

typedef struct IR_node
{
    cmd_code command;
    int imm_value;
    int reg_value;
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
//                                 X86 OP CODES
//-----------------------------------------------------------------------------
//                                  JUMP MASKS

static const uint32_t X86_MASK_JMP = 0x800F;              // j__ addr32
static const uint32_t X86_MASK_JNE = 0b0101;
static const uint32_t X86_MASK_JE  = 0b0100;
static const uint32_t X86_MASK_JA  = 0b0111;
static const uint32_t X86_MASK_JAE = 0b0011;
static const uint32_t X86_MASK_JB  = 0b0010;
static const uint32_t X86_MASK_JBE = 0b0110;
static const uint32_t X86_JMP      = 0xE9;

//-----------------------------------------------------------------------------
//                                  REGS MASKS

static const uint32_t MASK_X86_RAX = 0b1;
static const uint32_t MASK_X86_RBX = 0b111;
static const uint32_t MASK_X86_RCX = 0b11;
static const uint32_t MASK_X86_RDX = 0b101;

static const uint32_t X86_R_X[] = {MASK_X86_RAX, MASK_X86_RBX,
                                   MASK_X86_RCX, MASK_X86_RDX };

//-----------------------------------------------------------------------------
//                                  STACK UTILS
// PUSH

static const uint32_t X86_PUSH_RAX    = 0x50;             // push rax
static const uint32_t X86_PUSH_R13    = 0x5541;           // push r13
static const uint64_t X86_PUSHA       = 0x505152535241;   // pusha
static const uint32_t X86_SUB_RSP     = 0x08EC8348;       // sub rsp, 8
static const uint32_t X86_MOV_R_X_STK = 0x24008B48;       // mov r_x, [rsp]

// :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :
// POP

static const uint32_t X86_PUSH_RBP = 0x55;                // push rbp
static const uint32_t X86_POP_RBP  = 0x5D;                // pop rbp
static const uint64_t X86_POPA     = 0x5A415B5A5958;      // popa
static const uint32_t X86_ADD_RSP  = 0x08C48348;          // add rsp, 8

// :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :
// PROLOGUE UTILS

static const uint32_t X86_MOV_RBP_RSP = 0xE58948;         // mov rbp, rsp
static const uint32_t X86_MOV_RSP_RBP = 0xEC8948;         // mov rbp, rsp
static const uint32_t X86_ALIGN_STK   = 0xF0E48348;       // and rsp, ~0xF

// :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :
// XMM UTILS

static const uint32_t X86_CMP_XMM0_XMM1 = 0xC12E0F66;     // cmp xmm0, xmm1
static const uint64_t X86_MOV_XMM0_STK  = 0x002444100FF2; // movsd xmm0, [rsp]
static const uint64_t X86_MOV_XMM1_STK  = 0x08244C100FF2; // movsd xmm1, [rsp+8]
static const uint64_t X86_MOV_STK_XMM0  = 0x002444110FF2; // movsd [rsp], xmm0
static const uint64_t X86_MOV_STK_XMM1  = 0x00244C110FF2; // movsd [rsp], xmm1
static const uint64_t X86_MOV_MEM_XMM0  = 0x0045110F41F2; // movsd [r13], xmm0

// :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :
// PROCEDURES UTILS

static const uint32_t X86_LEA_RDI_RSP = 0x243C8D48;       // lea rdi, [rsp]
static const uint32_t X86_CALL        = 0xE8;             // call, imm64
static const uint32_t X86_RET         = 0xC3;             // ret

//-----------------------------------------------------------------------------
//                             ARITHM OPERATIONS

static const uint32_t X86_ADDSD            = 0xC8580FF2;  // addsd xmm1, xmm0
static const uint32_t X86_SUBSD            = 0xC85C0FF2;  // subsd xmm1, xmm0
static const uint32_t X86_MULSD            = 0xC8590FF2;  // mulsd xmm1, xmm0
static const uint32_t X86_DIVSD            = 0xC85E0FF2;  // divsd xmm1, xmm0
static const uint32_t X86_SQRTPD           = 0xC0510F66;  // sqrtpd xmm0, xmm0
static const uint32_t MASK_X86_ADD_R13_R_X = 0xC10149;    // add r13, r_x
static const uint32_t X86_MUL_R13_8        = 0x03E5C149;  // shl r13, 8
static const uint32_t X86_ADD_R13_R12      = 0xE5014D;    // add r13, r12

//-----------------------------------------------------------------------------
//                                 MOV IMM

static const uint32_t X86_MOV_R10 = 0xBA49;               // mov r10, imm64
static const uint32_t X86_MOV_R12 = 0xBC49;               // mov r12, imm64
static const uint32_t X86_MOV_R13 = 0xBD49;               // mov r13, imm64

//-----------------------------------------------------------------------------
//                                 MEM UTILS

static const uint32_t X86_MOV_R13_R13 = 0x006D8B4D;       // mov r13, [r13]

//-----------------------------------------------------------------------------
//                               SIZES OF OP CODES

enum OP_SIZES
{   
     X86_PUSH_RAX_SIZE         = 1,             
     X86_PUSH_R13_SIZE         = 2,         
     X86_PUSHA_SIZE            = 6,  
     X86_SUB_RSP_SIZE          = 4,      
     X86_MOV_R_X_STK_SIZE      = 4,     
     X86_PUSH_RBP_SIZE         = 1,               
     X86_POP_RBP_SIZE          = 1,              
     X86_POPA_SIZE             = 6,      
     X86_ADD_RSP_SIZE          = 4,     
     X86_MOV_RBP_RSP_SIZE      = 3,        
     X86_MOV_RSP_RBP_SIZE      = 3,      
     X86_ALIGN_STK_SIZE        = 4,      
     X86_CMP_XMM0_XMM1_SIZE    = 4,   
     X86_MOV_XMM0_STK_SIZE     = 6, 
     X86_MOV_XMM1_STK_SIZE     = 6, 
     X86_MOV_STK_XMM0_SIZE     = 6, 
     X86_MOV_STK_XMM1_SIZE     = 6, 
     X86_MOV_MEM_XMM0_SIZE     = 6, 
     X86_LEA_RDI_RSP_SIZE      = 4,      
     X86_CALL_SIZE             = 1,            
     X86_RET_SIZE              = 1,            
     X86_ADDSD_SIZE            = 4,  
     X86_SUBSD_SIZE            = 4, 
     X86_MULSD_SIZE            = 4,  
     X86_DIVSD_SIZE            = 4,  
     X86_SQRTPD_SIZE           = 4, 
     MASK_X86_ADD_R13_R_X_SIZE = 3,    
     X86_MUL_R13_8_SIZE        = 4,  
     X86_ADD_R13_R12_SIZE      = 3,    
     X86_MOV_R10_SIZE          = 2,              
     X86_MOV_R12_SIZE          = 2,              
     X86_MOV_R13_SIZE          = 2,               
     X86_MOV_R13_R13_SIZE      = 4,
     X86_JMP_SIZE              = 1,  
     X86_OPSD_SIZE             = 4,  
     X86_MASK_JMP_SIZE         = 2,
};

//-----------------------------------------------------------------------------

#endif // BT_COMMON_H