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

     MASK_R8  = 0b000, // ?
     MASK_R9  = 0b001,
     MASK_R10 = 0b010,
     MASK_R11 = 0b011,
     MASK_R12 = 0b100,
     MASK_R13 = 0b101,
     MASK_R14 = 0b110,
     MASK_R15 = 0b111,
} REGS_MASKS; 

//-----------------------------------------------------------------------------

typedef enum SSE_REGS_MASKS
{
     //
     //
     //
} SSE_REGS_MASKS;

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
     OP_CONDITIONAL_JMP = 0x800F, // j__ addr32
     OP_JMP             = 0xE9,

     OP_PUSH_REG        = 0x5040,
     OP_POP_REG         = 0x5840,

     OP_SUB_REG_IMM     = 0xEC8148, // PREFIX?
     OP_ADD_REG_IMM     = 0xC48148,

     OP_MOV_REG_MEM     = 0x8B4D,
     OP_MOV_MEM_REG     = 0x894D,
     OP_MOV_REG_REG     = 0x894D,

     OP_ALIGN_STK       = 0xF0E48348,

     OP_MOVSD_REG_MEM   = 0x0FF2,
     OP_MOVSD_MEM_REG   = 0x0FF2,
     OP_MOVSD_REG_REG   = 0x0FF2,
     OP_UCOMISD_REG_REG = 0x0F66,

     OP_CALL            = 0xE8,
     OP_RET             = 0xC3,
     OP_LEA             = 0x8D4D,

     OP_ADDSD_REG_REG = ,
     OP_SUBSD_REG_REG = ,
     OP_MULSD_REG_REG = ,
     OP_DIVSD_REG_REG = ,
     OP_SQRTPD_REG = ,

     OP_MOV_REG_IMM = 
} OP_CODES;

//-----------------------------------------------------------------------------
//                                  REGS MASKS

static const uint32_t X64_R_X[] = {MASK_X64_RAX, MASK_X64_RBX,
                                   MASK_X64_RCX, MASK_X64_RDX };

//-----------------------------------------------------------------------------
//                                  STACK UTILS
// PUSH

static const uint64_t X64_PUSHA       = 0x505152535241;   // pusha // ?
static const uint64_t X64_POPA         = 0x5A415B5A5958;      // popa

static const uint32_t X64_CMP_XMM0_XMM1 = 0xC12E0F66;     // cmp xmm0, xmm1
static const uint64_t X64_MOV_XMM0_STK  = 0x002444100FF2; // movsd xmm0, [rsp]
static const uint64_t X64_MOV_XMM1_STK  = 0x08244C100FF2; // movsd xmm1, [rsp+8]
static const uint64_t X64_MOV_STK_XMM0  = 0x002444110FF2; // movsd [rsp], xmm0
static const uint64_t X64_MOV_STK_XMM1  = 0x00244C110FF2; // movsd [rsp], xmm1
static const uint64_t X64_MOV_MEM_XMM0  = 0x0045110F41F2; // movsd [r13], xmm0

// :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :
// PROCEDURES UTILS

static const uint32_t X64_LEA_RDI_RSP = 0x243C8D48;       // lea rdi, [rsp]
static const uint32_t X64_CALL        = 0xE8;             // call, imm64
static const uint32_t X64_RET         = 0xC3;             // ret

//-----------------------------------------------------------------------------
//                             ARITHM OPERATIONS

static const uint32_t X64_ADDSD            = 0xC8580FF2;  // addsd xmm1, xmm0
static const uint32_t X64_SUBSD            = 0xC85C0FF2;  // subsd xmm1, xmm0
static const uint32_t X64_MULSD            = 0xC8590FF2;  // mulsd xmm1, xmm0
static const uint32_t X64_DIVSD            = 0xC85E0FF2;  // divsd xmm1, xmm0

static const uint32_t X64_SQRTPD           = 0xC0510F66;  // sqrtpd xmm0, xmm0
static const uint32_t MASK_X64_ADD_R13_R_X = 0xC10149;    // add r13, r_x
static const uint32_t X64_MUL_R13_8        = 0x03E5C149;  // shl r13, 8
static const uint32_t X64_ADD_R13_R12      = 0xE5014D;    // add r13, r12

//-----------------------------------------------------------------------------
//                                 MOV IMM

static const uint32_t X64_MOV_R10 = 0xBA49;               // mov r10, imm64
static const uint32_t X64_MOV_R12 = 0xBC49;               // mov r12, imm64
static const uint32_t X64_MOV_R13 = 0xBD49;               // mov r13, imm64

//-----------------------------------------------------------------------------
//                                 MEM UTILS

static const uint32_t X64_MOV_R13_R13 = 0x006D8B4D;       // mov r13, [r13]

//-----------------------------------------------------------------------------
//                               SIZES OF OP CODES

enum OP_SIZES
{   
     X64_PUSH_RAX_SIZE         = 1,             
     X64_PUSH_R13_SIZE         = 2,         
     X64_PUSHA_SIZE            = 6,  
     X64_SUB_RSP_SIZE          = 4,      
     X64_MOV_R_X_STK_SIZE      = 4,     
     X64_PUSH_RBP_SIZE         = 1,               
     X64_POP_RBP_SIZE          = 1,              
     X64_POPA_SIZE             = 6,      
     X64_ADD_RSP_SIZE          = 4,     
     X64_MOV_RBP_RSP_SIZE      = 3,        
     X64_MOV_RSP_RBP_SIZE      = 3,      
     X64_ALIGN_STK_SIZE        = 4,      
     X64_CMP_XMM0_XMM1_SIZE    = 4,   
     X64_MOV_XMM0_STK_SIZE     = 6, 
     X64_MOV_XMM1_STK_SIZE     = 6, 
     X64_MOV_STK_XMM0_SIZE     = 6, 
     X64_MOV_STK_XMM1_SIZE     = 6, 
     X64_MOV_MEM_XMM0_SIZE     = 6, 
     X64_LEA_RDI_RSP_SIZE      = 4,      
     X64_CALL_SIZE             = 1,            
     OP_RET_SIZE               = 1,            
     X64_ADDSD_SIZE            = 4,  
     X64_SUBSD_SIZE            = 4, 
     X64_MULSD_SIZE            = 4,  
     X64_DIVSD_SIZE            = 4,  
     X64_SQRTPD_SIZE           = 4,    
     X64_MUL_R13_8_SIZE        = 4,  
     X64_ADD_R13_R12_SIZE      = 3,    
     X64_MOV_R10_SIZE          = 2,              
     X64_MOV_R12_SIZE          = 2,              
     X64_MOV_R13_SIZE          = 2,               
     X64_MOV_R13_R13_SIZE      = 4,
     X64_JMP_SIZE              = 1,  
     X64_OPSD_SIZE             = 4,  
     X64_MASK_JMP_SIZE         = 2,
     MASK_X64_ADD_R13_R_X_SIZE = 3, 
};

//-----------------------------------------------------------------------------

#endif //X64_CODES_H