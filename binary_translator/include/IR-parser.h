//! @file IR-parser.h

#ifndef IRPARSER_H
#define IRPARSER_H

//-----------------------------------------------------------------------------

#include "CPU/include/stack.h"
#include "COMMON/include/common.h"

//-----------------------------------------------------------------------------

#define Struct Processor
#define Name   cpu

//-----------------------------------------------------------------------------

typedef long long int signature;

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

enum OFFSETS
{
    OFFSET_CMD = 1,
    OFFSET_ARG = sizeof (signature),
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

typedef struct Bin_code
{
    char *buffer;
    int   size;
} Bin_code;

//-----------------------------------------------------------------------------

typedef struct IRcode
{

} IRcode;

//-----------------------------------------------------------------------------

void read_code_file  (Processor *cpu);

void handle_cmds     (Processor *cpu);

bool is_equal        (double a, double b);

void execute_cmd     (int curr_cmd,  double    *curr_arg, double arg_value,
                      int *curr_ptr, Processor *cpu                        );

void cpu_dump        (Processor *cpu);

void translateBinToIR ();

//-----------------------------------------------------------------------------

#endif //IRPARSER_H