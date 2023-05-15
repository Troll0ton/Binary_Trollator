//! @file IR-parser.h

#ifndef IRPARSER_H
#define IRPARSER_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

#define Struct Processor
#define Name   cpu

//-----------------------------------------------------------------------------

typedef double signature;
typedef double elem_t;

typedef unsigned int cmd_code;

//-----------------------------------------------------------------------------

#define DELETED_PAR -1
#define NOT_FOUND   -1

//-----------------------------------------------------------------------------

#define N(suffix)  NUM_OF_##suffix
#define O(suffix)  OFFSET_##suffix

//-----------------------------------------------------------------------------

enum CMD_CODES
{
    #define CMD_DEF(cmd, ...) \
    cmd,

    #include "processor/COMMON/include/codegen/codegen.h"

    #undef CMD_DEF
};

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

typedef struct Bin_code
{
    char *buffer;
    int   size;
} Bin_code;

//-----------------------------------------------------------------------------

typedef struct Ir_code
{
    cmd_code command;
    int  imm_value;
    int  reg_num;
    char ram_flag;
} Ir_code;

typedef struct Intrm_represent
{
    Ir_code *buffer;
    int size;
} Intrm_represent;

//-----------------------------------------------------------------------------

Bin_code *readCodeFile (FILE *code_file);

Intrm_represent *translateBinToIR (Bin_code *bin_code);

void handleBinCode (Intrm_represent *intrm_repres, Bin_code *bin_code);

void IrDump (Intrm_represent *intrm_repres);

//-----------------------------------------------------------------------------

#endif //IRPARSER_H