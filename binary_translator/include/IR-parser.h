//! @file IR-parser.h

#ifndef IRPARSER_H
#define IRPARSER_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

static const int jump_fill_error = 0xEAEAEA;

//-----------------------------------------------------------------------------

enum OFFSETS
{
    OFFSET_CMD = 1,
    OFFSET_ARG = sizeof (elem_t),
};

enum SIGNATURES
{
    SIGNATURE = 0xBACAFE,
};

enum CODE_INFO
{
    SIZE_DIFFERENCE       = 40,
    CODE_SIZE             = 2 * OFFSET_ARG,
    OFFSET_CODE_SIGNATURE = 2 * OFFSET_ARG,
};

enum BIT_MASKS
{
    MASK_IMM = 0x20,
    MASK_REG = 0x40,
    MASK_RAM = 0x80,
    MASK_CMD = 0x1F,
};

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