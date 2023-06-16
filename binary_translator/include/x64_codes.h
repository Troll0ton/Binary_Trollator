//! @file x64_codes.h

#ifndef X64_CODES_H
#define X64_CODES_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

// this enum contains masks of registers
// they later used in masking with complex ops
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
     
     MASK_R8  = 0b1000, 
     MASK_R9  = 0b1001,
     MASK_R10 = 0b1010,
     MASK_R11 = 0b1011,
     MASK_R12 = 0b1100,
     MASK_R13 = 0b1101,
     MASK_R14 = 0b1110,
     MASK_R15 = 0b1111,
} REGS_MASKS; 

//-----------------------------------------------------------------------------

// this enum contains masks of different types of conditional jumps
// it will used in masking with OP_CONDITIONAL_JMP
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
     // j__ (masking with ja, jae, jb, jbe, je, jne) addr32
     OP_CONDITIONAL_JMP = 0x800F,        

     // jmp addr32
     OP_JMP             = 0xE9,

     // push reg (MASKING with register)
     OP_PUSH_REG        = 0x5040,

     // pop reg (MASKING with register)
     OP_POP_REG         = 0x5840,

     // add reg imm32 (MASKING with register)
     OP_ADD_REG_IMM     = 0xC08148, 

     // sub reg imm32 (MASKING with register)
     OP_SUB_REG_IMM     = 0xE88148, 

     // mov rbp, rsp
     OP_MOV_RBP_RSP     = 0xE58948,    

     // mov rsp, rbp    
     OP_MOV_RSP_RBP     = 0xEC8948, 

     // and rsp, ~0xF
     OP_ALIGN_STK       = 0xF0E48348,

     // call addr64
     OP_CALL            = 0xE8,

     // ret
     OP_RET             = 0xC3,

     // lea rdi, [rsp]
     OP_LEA_RDI_STK_ARG = 0x243C8D48,

     // push rax, rbx, rcx, rdx, r12, r13
     OP_PUSHA           = 0x5541544152515350,

     // pop r13, r12, rdx, rcx, rbx, rax
     OP_POPA            = 0x585B595A5C415D41,

     // mov reg, imm64 (MASKING with register)
     OP_MOV_REG_IMM     = 0xB848,

     // add r13, reg (MASKING with register)
     OP_ADD_R13_REG     = 0xC50149,

     // shl reg, imm8 (MASKING with register) + imm8
     OP_SHL_REG         = 0xE0C148, 

     // mov r13, [r13]
     OP_MOV_R13_RAM     = 0x006D8B4D,     

     // mov reg, [rsp] (MASKING with register)
     OP_MOV_REG_STK     = 0x24048B48,

     // cmp xmm0, xmm1
     OP_CMP_XMM0_XMM1   = 0xC12E0F66,     

     // movsd xmm0, [rsp]
     OP_MOV_XMM0_STK    = 0x002444100FF2, 

     // movsd xmm1, [rsp+8]
     OP_MOV_XMM1_STK    = 0x08244C100FF2,

     // movsd [rsp], xmm0
     OP_MOV_STK_XMM0    = 0x002444110FF2, 

     // movsd [rsp], xmm1
     OP_MOV_STK_XMM1    = 0x00244C110FF2, 

     // movsd [r13], xmm0
     OP_MOV_MEM_XMM0    = 0x0045110F41F2, 

     // addsd xmm1, xmm0
     OP_ADDSD_XMM1_XMM0 = 0xC8580FF2,     

     // subsd xmm1, xmm0
     OP_SUBSD_XMM1_XMM0 = 0xC85C0FF2,

     // mulsd xmm1, xmm0
     OP_MULSD_XMM1_XMM0 = 0xC8590FF2,     

     // divsd xmm1, xmm0
     OP_DIVSD_XMM1_XMM0 = 0xC85E0FF2, 

     // sqrtpd xmm0, xmm0 
     OP_SQRTPD_XMM0     = 0xC0510F66, 

     // cvttsd2si reg, xmm0
     OP_CVTTSD2SI_REG   = 0xC02C0F48F2,     
} OP_CODES;

//-----------------------------------------------------------------------------

// this enum contains sizes of different opcodes 
enum OP_SIZES
{   
     OP_PUSH_REG_SIZE         = 2, 
     OP_POP_REG_SIZE          = 2, 
     OP_PUSHA_SIZE            = 8,
     OP_POPA_SIZE             = 8,
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
     OP_ADDSD_XMM1_XMM0_SIZE  = 4,
     OP_SUBSD_XMM1_XMM0_SIZE  = 4,
     OP_MULSD_XMM1_XMM0_SIZE  = 4,
     OP_DIVSD_XMM1_XMM0_SIZE  = 4,
     OP_SQRTPD_XMM0_SIZE      = 4,
     OP_LEA_RDI_STK_ARG_SIZE  = 4,
     OP_ALIGN_STK_SIZE        = 4,
     OP_CALL_SIZE             = 1,
     OP_CMP_XMM0_XMM1_SIZE    = 4,
     OP_CONDITIONAL_JMP_SIZE  = 2,
     OP_JMP_SIZE              = 1,
     OP_MOV_STK_XMM0_SIZE     = 6,
     OP_CVTTSD2SI_REG_SIZE    = 5,
};

//-----------------------------------------------------------------------------

// this enum contains position of masks in common opcodes
enum MASKS_POS
{
     POS_OP_PUSH_REG      = 8,
     POS_OP_POP_REG       = 8,
     POS_OP_MOV_REG_IMM   = 8,
     POS_OP_MASK_JMP      = 8,
     POS_OP_ADD_R13_REG   = 19,
     POS_OP_SHL_REG       = 16,
     POS_OP_MOV_REG_STK   = 19,
     POS_OP_ADD_REG_IMM   = 16,
     POS_OP_SUB_REG_IMM   = 16,
     POS_OP_CVTTSD2SI_REG = 35,
};

//-----------------------------------------------------------------------------

// this is position of mask R_MUM in opcode
enum MASK_R_POS
{
     MASK_R_OP_PUSH_REG      = 0,
     MASK_R_OP_POP_REG       = 0,
     MASK_R_OP_MOV_REG_IMM   = 0,
     MASK_R_OP_MASK_JMP      = 0,
     MASK_R_OP_ADD_R13_REG   = 2,
     MASK_R_OP_ADD_REG_IMM   = 0,
     MASK_R_OP_SUB_REG_IMM   = 0,
     MASK_R_OP_SHL_REG       = 0,
     MASK_R_OP_MOV_REG_STK   = 0,
     MASK_R_OP_CVTTSD2SI_REG = 0,
};

//-----------------------------------------------------------------------------

#endif //X64_CODES_H