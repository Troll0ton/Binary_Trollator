//! @file IR-parser.h

#ifndef IRPARSER_H
#define IRPARSER_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"
#include "binary_translator/include/input_output.h" 

//-----------------------------------------------------------------------------

#define SIZE_OF_INSTRUCTION 1
#define SIZE_OF_ARGUMENTS_SPECIFIER sizeof (double)

#define GUEST_CODE_SIGNATURE 0xBACAFE
#define SIZE_OF_CODE_SIGNATURE sizeof (int)

#define ERROR_ERROR_JUMP_TARGET_EMPTY 0xEAEAEA

//-----------------------------------------------------------------------------

enum GUEST_CODE_BIT_MASKS
{
    MASK_IMM = 0x20,
    MASK_REG = 0x40,
    MASK_MEM = 0x80,
    MASK_CMD = 0x1F,
};

enum IR_REGS_IDENTIFIERS
{
    IR_IDENTIFIER_RAX = 1,
    IR_IDENTIFIER_RBX = 2,
    IR_IDENTIFIER_RCX = 3,
    IR_IDENTIFIER_RDX = 4,
};

//-----------------------------------------------------------------------------

typedef struct Guest_code
{
    char *buffer;
    int   size;
} Guest_code;

//-----------------------------------------------------------------------------

typedef union Imm_val
{
    double num;
    int    target;
} Imm_val;

typedef union Address
{
    int   guest;
    char *x64;
} Address;

//-----------------------------------------------------------------------------

typedef struct IR_node
{
    uint32_t     command;
    Imm_val      imm_val;
    Address      address;
    uint8_t      reg_num;
    unsigned int memory_flag : 1;
} IR_node;

//-----------------------------------------------------------------------------

typedef struct IR
{
    IR_node *buffer;
    int      size;
} IR;

//-----------------------------------------------------------------------------

Guest_code *readCodeFile (FILE *code_file, FILE *log_file);

IR *translateGuestToIr (Guest_code *guest_code, FILE *log_file);

void handleGuestCode (IR *ir, Guest_code *guest_code, FILE *log_file);

void translateGuestJmpTargets (IR *ir, FILE *log_file);

void searchForTarget (IR *ir, IR_node *ir_node, FILE *log_file);

int handleBinMask (IR_node *ir_node, Guest_code *bin_code, int curr_pos);

void irDump (IR *ir, FILE *log_file);

void irDtor (IR *ir);

void guestCodeDtor (Guest_code *bin_code);

//-----------------------------------------------------------------------------

#endif //IRPARSER_H