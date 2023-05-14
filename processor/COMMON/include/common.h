#ifndef   COMMON_H
#define   COMMON_H

//-----------------------------------------------------------------------------

#include <stdint.h>

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

    #include "COMMON/include/codegen/codegen.h"

    #undef CMD_DEF
};

//-----------------------------------------------------------------------------

#define CJMP(...)                                    \
    double second_number = stack_pop (&cpu->Stk);    \
    double first_number  = stack_pop (&cpu->Stk);    \
                                                     \
    if(__VA_ARGS__)                                  \
    {                                                \
        int pos_ch = arg_value;                      \
        curr_pos = pos_ch - 1;                       \
    }                                                \
                                                     \
    stack_push (&cpu->Stk, first_number);            \
    stack_push (&cpu->Stk, second_number);

//-----------------------------------------------------------------------------

typedef double elem_t;

//-----------------------------------------------------------------------------

enum OFFSETS
{
    OFFSET_CMD = 1,
    OFFSET_ARG = sizeof(elem_t),
};

//-----------------------------------------------------------------------------

enum SIGNATURES
{
    SIGNATURE           = 0xBACAFE,
    SIGNATURE_DESTROYED = 0xDEADAC,
};

//-----------------------------------------------------------------------------

enum code_INFO
{
    SIZE_DIFFERENCE       = 40,
    CODE_SIZE             = 2 * OFFSET_ARG,
    OFFSET_CODE_SIGNATURE = 2 * OFFSET_ARG,
};

//-----------------------------------------------------------------------------

enum BIT_MASKS
{
    MASK_IMM = 0x20,
    MASK_REG = 0x40,
    MASK_RAM = 0x80,
    MASK_CMD = 0x1F,
};

//-----------------------------------------------------------------------------

enum POISONS
{
    POISON_STK = 0xBADADD,
};

//-----------------------------------------------------------------------------

enum codeS_OF_ERROR
{
    ERROR_CTOR  = 1,
    ERROR_ASM   = 2,
    ERROR_CPU   = 3,
    ERROR_DASM  = 4,
};

//-----------------------------------------------------------------------------

#endif //COMMON_H
