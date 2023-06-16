//! @file IR-parser.h

#ifndef IRPARSER_H
#define IRPARSER_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

enum BYTE_CODE_OFFSETS
{
    OFFSET_CMD = 1,
    OFFSET_ARG = sizeof (elem_t),
};

//-----------------------------------------------------------------------------

enum BYTE_CODE_SIGNATURE_INFO
{
    OFFSET_CODE_SIGNATURE = 2 * OFFSET_ARG,
    SIGNATURE             = 0xBACAFE,
    DESTROYED             = 0xBADBAD,
};

//-----------------------------------------------------------------------------

enum BYTE_CODE_BIT_MASKS
{
    MASK_IMM = 0x20,
    MASK_REG = 0x40,
    MASK_RAM = 0x80,
    MASK_CMD = 0x1F,
};

//-----------------------------------------------------------------------------

// This is actually (in my format) byte code
typedef struct Troll_code
{
    char *buffer;
    int   size;
} Troll_code;

//-----------------------------------------------------------------------------

enum IR_INFO
{
    // This value indicates that there is a mistake in target address
    JUMP_TARGET_EMPTY = 0xEAEAEA,

    // IR register is taken from bincode + 1
    IR_REG = 1,

    // MASKS IN IR
    IR_RAX = 1,
    IR_RBX = 2,
    IR_RCX = 3,
    IR_RDX = 4,
};

//-----------------------------------------------------------------------------

typedef struct IR_node
{
    cmd_code command;
    int      imm_value;
    int      reg_value;
    char     ram_flag;
    int      troll_pos;
    char    *x64_pos;
} IR_node;

//-----------------------------------------------------------------------------

typedef struct IR
{
    IR_node *buffer;
    int      size;
} IR;

//-----------------------------------------------------------------------------

Troll_code *readCodeFile (FILE *code_file);

IR *translateBinToIr (Troll_code *bin_code);

void handleBinCode (IR *ir, Troll_code *bin_code);

int translateJmpTargetsIR (IR *ir);

int handleTrollMask (IR_node *ir_node, Troll_code *bin_code, int curr_pos);

void IrDump (IR *ir);

void IrDtor (IR *ir);

void BinCodeDtor (Troll_code *bin_code);

//-----------------------------------------------------------------------------

#endif //IRPARSER_H