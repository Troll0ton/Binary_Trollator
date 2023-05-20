//! @file IR-parser.h

#ifndef IRPARSER_H
#define IRPARSER_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

#define DELETED_PAR 0xBADBED
#define NOT_FOUND   -1

//-----------------------------------------------------------------------------

#define N(suffix) NUM_OF_##suffix
#define O(suffix) OFFSET_##suffix

//-----------------------------------------------------------------------------

//cringe
#define IS_JUMP(i)                                                     \
    (i == JMP) || (i == JE) || (i == JNE) || (i == JBE) || (i == JB) || \
    (i == JA) || (i == JAE)

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

Bin_code *readCodeFile (FILE *code_file);

IR *translateBinToIR (Bin_code *bin_code);

void handleBinCode (IR *intrm_repres, Bin_code *bin_code);

void IrDump (IR *intrm_repres);

void IntrmRepresentDtor (IR *intrm_repres);

void BinCodeDtor (Bin_code *bin_code);

//-----------------------------------------------------------------------------

#endif //IRPARSER_H