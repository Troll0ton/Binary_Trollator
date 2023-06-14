//! @file x64_codes.h

#ifndef X64_CODES_H
#define X64_CODES_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

typedef enum REGS_MASKS
{
     MASK_RAX = 0b000,
     MASK_RBX = 0b011,
     MASK_RCX = 0b001,
     MASK_RDX = 0b010,

     MASK_RSI = 0b110,
     MASK_RDI = 0b111,
     MASK_RSP = 0b100,
     MASK_RBP = 0b101,
     
     MASK_R   = 1,
     MASK_R8  = 0b000, 
     MASK_R9  = 0b001,
     MASK_R10 = 0b010,
     MASK_R11 = 0b011,
     MASK_R12 = 0b100,
     MASK_R13 = 0b101,
     MASK_R14 = 0b110,
     MASK_R15 = 0b111,
} REGS_MASKS; 

//-----------------------------------------------------------------------------

typedef enum MASKS_POS
{
     POS_MASK_REG    = 8,
     POS_MASK_JMP    = 8,
     POS_ADD_R13_REG = 18,
     POS_SHL_REG     = 16,
     POS_MOV_REG_STK = 18,
     POS_ADD_REG_IMM = 16,
} MASKS_POS;

//-----------------------------------------------------------------------------

typedef enum JUMP_MASKS
{             
     MASK_JNE = 0b0101,
     MASK_JE  = 0b0100,
     MASK_JA  = 0b0111,
     MASK_JAE = 0b0011,
     MASK_JB  = 0b0010,
     MASK_JBE = 0b0110,
} JUMP_MASKS;

//-----------------------------------------------------------------------------

typedef enum OP_CODES : uint64_t
{
     OP_CONDITIONAL_JMP = 0x800F,        // j__ addr32
     OP_JMP             = 0xE9,
     OP_PUSH_REG        = 0x5040,
     OP_POP_REG         = 0x5840,
     OP_SUB_REG_IMM     = 0xEC8148, 
     OP_ADD_REG_IMM     = 0xC08148,
     OP_MOV_RBP_RSP     = 0xE58948,        
     OP_MOV_RSP_RBP     = 0xEC8948, 
     OP_ALIGN_STK       = 0xF0E48348,
     OP_CALL            = 0xE8,
     OP_RET             = 0xC3,
     OP_LEA_RDI_STK_ARG = 0x243C8D48,
     OP_PUSHA           = 0x505152535241,
     OP_POPA            = 0x5A415B5A5958,
     OP_MOV_REG_IMM     = 0xB848,
     OP_ADD_R13_REG     = 0xC10149,
     OP_SHL_REG         = 0xE0C148, 
     OP_MOV_R13_RAM     = 0x006D8B4D,     
     OP_MOV_REG_STK     = 0x24008B48,
     OP_CMP_XMM0_XMM1   = 0xC12E0F66,     // cmp    xmm0,  xmm1
     OP_MOV_XMM0_STK    = 0x002444100FF2, // movsd  xmm0,  [rsp]
     OP_MOV_XMM1_STK    = 0x08244C100FF2, // movsd  xmm1,  [rsp+8]
     OP_MOV_STK_XMM0    = 0x002444110FF2, // movsd  [rsp], xmm0
     OP_MOV_STK_XMM1    = 0x00244C110FF2, // movsd  [rsp], xmm1
     OP_MOV_MEM_XMM0    = 0x0045110F41F2, // movsd  [r13], xmm0
     OP_ADDSD_XMM1_XMM0 = 0xC8580FF2,     // addsd  xmm1,  xmm0
     OP_SUBSD_XMM1_XMM0 = 0xC85C0FF2,     // subsd  xmm1,  xmm0
     OP_MULSD_XMM1_XMM0 = 0xC8590FF2,     // mulsd  xmm1,  xmm0
     OP_DIVSD_XMM1_XMM0 = 0xC85E0FF2,     // divsd  xmm1,  xmm0
     OP_SQRTPD_XMM0     = 0xC0510F66,     // sqrtpd xmm0,  xmm0    
} OP_CODES;

//-----------------------------------------------------------------------------

enum OP_SIZES
{   
     OP_PUSH_REG_SIZE         = 2, 
     OP_POP_REG_SIZE          = 2, 
     OP_PUSHA_SIZE            = 6,
     OP_POPA_SIZE             = 6,
     OP_MOV_RBP_RSP_SIZE      = 3,   
     OP_MOV_RSP_RBP_SIZE      = 3,
     OP_RET_SIZE              = 1,
     OP_MOV_REG_IMM_SIZE      = 2,
     OP_ADD_R13_REG_SIZE      = 3, 
     OP_SHL_REG_SIZE          = 3,
     OP_MOV_R13_RAM_SIZE      = 4,
     OP_MOV_REG_STK_SIZE      = 4,
     OP_ADD_REG_IMM_SIZE      = 3,
     OP_SUB_REG_IMM_SIZE      = 3,
     OP_MOV_MEM_XMM0_SIZE     = 6,
     OP_MOV_XMM0_STK_SIZE     = 6,
     OP_MOV_XMM1_STK_SIZE     = 6,
     OP_MOV_STK_XMM1_SIZE     = 6,
     OP_ARITHM_SIZE           = 4,
     OP_LEA_RDI_STK_ARG_SIZE  = 4,
     OP_ALIGN_STK_SIZE        = 4,
     OP_CALL_SIZE             = 1,
     OP_CMP_XMM0_XMM1_SIZE    = 4,
     OP_CONDITIONAL_JMP_SIZE  = 2,
     OP_JMP_SIZE              = 1,
     OP_MOV_STK_XMM0_SIZE     = 6,
};

//-----------------------------------------------------------------------------

#endif //X64_CODES_H