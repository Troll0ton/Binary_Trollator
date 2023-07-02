//! @file X64_codes.h

#ifndef X64_CODES_H
#define X64_CODES_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

// this enum contains masks of registers
// they later used in masking with complex ops
typedef enum REGS_IDS
{
     RAX_ID = 0b000,
     RBX_ID = 0b011,
     RCX_ID = 0b001,
     RDX_ID = 0b010,
     RSI_ID = 0b110,
     RDI_ID = 0b111,
     RSP_ID = 0b100,
     RBP_ID = 0b101,
     R8_ID  = 0b1000, 
     R9_ID  = 0b1001,
     R10_ID = 0b1010,
     R11_ID = 0b1011,
     R12_ID = 0b1100,
     R13_ID = 0b1101,
     R14_ID = 0b1110,
     R15_ID = 0b1111,
} REGS_IDS; 

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

// ADD_OP_CODE()

typedef enum OP_CODES : uint64_t
{
     // j__ (masking with ja, jae, jb, jbe, je, jne) addr32
     CONDITIONAL_JMP_OPCODE = 0x800F,        

     // jmp addr32
     JMP_OPCODE             = 0xE9,

     // push reg (MASKING with register)
     PUSH_REG_OPCODE        = 0x5040,

     // pop reg (MASKING with register)
     POP_REG_OPCODE         = 0x5840,

     // add reg imm32 (MASKING with register)
     ADD_REG_IMM_OPCODE     = 0xC08148, 

     // sub reg imm32 (MASKING with register)
     SUB_REG_IMM_OPCODE     = 0xE88148, 

     // mov rbp, rsp
     MOV_RBP_RSP_OPCODE     = 0xE58948,    

     // mov rsp, rbp    
     MOV_RSP_RBP_OPCODE     = 0xEC8948, 

     // and rsp, ~0xF
     ALIGN_STK_OPCODE       = 0xF0E48348,

     // call addr64
     CALL_OPCODE            = 0xE8,

     // ret
     RET_OPCODE             = 0xC3,

     // lea rdi, [rsp]
     LEA_RDI_STK_ARG_OPCODE = 0x243C8D48,

     // push rax, rbx, rcx, rdx, r12, r10
     PUSHA_OPCODE           = 0x5241544152515350,

     // pop r10, r12, rdx, rcx, rbx, rax
     POPA_OPCODE            = 0x585B595A5C415A41,

     // mov reg, imm64 (MASKING with register)
     MOV_REG_IMM_OPCODE     = 0xB848,

     // add r13, reg (MASKING with register)
     ADD_R13_REG_OPCODE     = 0xC50149,

     // shl reg, imm8 (MASKING with register) + imm8
     SHL_REG_OPCODE         = 0xE0C148, 

     // mov r13, [r13]
     MOV_R13_RAM_OPCODE     = 0x006D8B4D,     

     // mov reg, [rsp] (MASKING with register)
     MOV_REG_STK_OPCODE     = 0x24048B48,

     // cmp xmm0, xmm1
     CMP_XMM0_XMM1_OPCODE   = 0xC12E0F66,     

     // movsd xmm0, [rsp]
     MOV_XMM0_STK_OPCODE    = 0x002444100FF2, 

     // movsd xmm1, [rsp+8]
     MOV_XMM1_STK_OPCODE    = 0x08244C100FF2,

     // movsd [rsp], xmm0
     MOV_STK_XMM0_OPCODE    = 0x002444110FF2, 

     // movsd [rsp], xmm1
     MOV_STK_XMM1_OPCODE    = 0x00244C110FF2, 

     // movsd [r13], xmm0
     MOV_MEM_XMM0_OPCODE    = 0x0045110F41F2, 

     // addsd xmm1, xmm0
     ADDSD_XMM1_XMM0_OPCODE = 0xC8580FF2,     

     // subsd xmm1, xmm0
     SUBSD_XMM1_XMM0_OPCODE = 0xC85C0FF2,

     // mulsd xmm1, xmm0
     MULSD_XMM1_XMM0_OPCODE = 0xC8590FF2,     

     // divsd xmm1, xmm0
     DIVSD_XMM1_XMM0_OPCODE = 0xC85E0FF2, 

     // sqrtpd xmm0, xmm0 
     SQRTPD_XMM0_OPCODE     = 0xC0510F66, 

     // cvttsd2si reg, xmm0
     CVTTSD2SI_REG_OPCODE   = 0xC02C0F48F2,  

     // syscall
     SYSCALL_OPCODE         = 0x050F,   
} OP_CODES;

//-----------------------------------------------------------------------------

// this enum contains sizes of different opcodes 
enum OP_SIZES
{   
     PUSH_REG_SIZE         = 2, 
     POP_REG_SIZE          = 2, 
     PUSHA_SIZE            = 8,
     POPA_SIZE             = 8,
     MOV_RBP_RSP_SIZE      = 3,   
     MOV_RSP_RBP_SIZE      = 3,
     RET_SIZE              = 1,
     MOV_REG_IMM_SIZE      = 2,
     ADD_R13_REG_SIZE      = 3, 
     SHL_REG_SIZE          = 3,
     MOV_R13_RAM_SIZE      = 4,
     MOV_REG_STK_SIZE      = 4,
     ADD_REG_IMM_SIZE      = 3,
     SUB_REG_IMM_SIZE      = 3,
     MOV_MEM_XMM0_SIZE     = 6,
     MOV_XMM0_STK_SIZE     = 6,
     MOV_XMM1_STK_SIZE     = 6,
     MOV_STK_XMM1_SIZE     = 6,
     ADDSD_XMM1_XMM0_SIZE  = 4,
     SUBSD_XMM1_XMM0_SIZE  = 4,
     MULSD_XMM1_XMM0_SIZE  = 4,
     DIVSD_XMM1_XMM0_SIZE  = 4,
     SQRTPD_XMM0_SIZE      = 4,
     LEA_RDI_STK_ARG_SIZE  = 4,
     ALIGN_STK_SIZE        = 4,
     CALL_SIZE             = 1,
     CMP_XMM0_XMM1_SIZE    = 4,
     CONDITIONAL_JMP_SIZE  = 2,
     JMP_SIZE              = 1,
     MOV_STK_XMM0_SIZE     = 6,
     CVTTSD2SI_REG_SIZE    = 5,
     SYSCALL_SIZE          = 2,
};

//-----------------------------------------------------------------------------

// this enum contains position of masks in common opcodes
enum REG_ID_POS
{
     PUSH_REG_ID_POS      = 8,
     POP_REG_ID_POS       = 8,
     MOV_REG_IMM_ID_POS   = 8,
     MASK_JMP_ID_POS      = 8,
     ADD_R13_REG_ID_POS   = 19,
     SHL_REG_ID_POS       = 16,
     MOV_REG_STK_ID_POS   = 19,
     ADD_REG_IMM_ID_POS   = 16,
     SUB_REG_IMM_ID_POS   = 16,
     CVTTSD2SI_REG_ID_POS = 35,
};

//-----------------------------------------------------------------------------

enum REG_BIT_POS
{
     PUSH_REG_REG_BIT_POS      = 0,
     POP_REG_REG_BIT_POS       = 0,
     MOV_REG_IMM_REG_BIT_POS   = 0,
     MASK_JMP_REG_BIT_POS      = 0,
     ADD_R13_REG_REG_BIT_POS   = 2,
     ADD_REG_IMM_REG_BIT_POS   = 0,
     SUB_REG_IMM_REG_BIT_POS   = 0,
     SHL_REG_REG_BIT_POS       = 0,
     MOV_REG_STK_REG_BIT_POS   = 0,
     CVTTSD2SI_REG_REG_BIT_POS = 0,
};

//-----------------------------------------------------------------------------

#endif //X64_CODES_H