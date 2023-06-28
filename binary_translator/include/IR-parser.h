//! @file IR-parser.h

#ifndef IRPARSER_H
#define IRPARSER_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"
#include "binary_translator/include/input_output.h" 

//-----------------------------------------------------------------------------

enum BYTE_CODE_OFFSETS
{
    SIZE_OF_INSTRUCTION        = 1,
    SIZE_OF_ARGUMENT_SPECIFIER = sizeof(double),
};

//-----------------------------------------------------------------------------

enum BYTE_CODE_SIGNATURE_INFO
{
    GUEST_CODE_SIGNATURE  = 0xBACAFE,
    OFFSET_CODE_SIGNATURE = sizeof(int),
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
typedef struct Guest_code
{
    char *buffer;
    int   size;
} Guest_code;

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

typedef union imm_val
{
    double num;
    int offset;
} imm_val;

typedef union address
{
    int guest;
    char *x64;
} address;

//-----------------------------------------------------------------------------

typedef struct IR_node
{
    cmd_code command;
    imm_val  imm_val;
    int      reg_num  : 2;
    int      ram_flag : 1;

    address  address;
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

void irDump (IR *ir);

void irDtor (IR *ir);

void guestCodeDtor (Guest_code *bin_code);

//-----------------------------------------------------------------------------

#endif //IRPARSER_H