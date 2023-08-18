//! @file IR-parser.h

#ifndef IR_PARSER_H
#define IR_PARSER_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"
#include "binary_translator/include/input_output.h" 

//-----------------------------------------------------------------------------

enum GUEST_CODE_INFO
{
    SIZE_OF_OPCODE         = 1,
    SIZE_OF_ARGUMENT_SPECIFIER = sizeof (double),
    GUEST_CODE_SIGNATURE        = 0xBACAFE,
    SIZE_OF_CODE_SIGNATURE      = sizeof (int),
};

//-----------------------------------------------------------------------------

enum IR_INFO
{
    IR_INIT_SIZE    = 100,
    IR_INCREASE_PAR = 100,
};

//-----------------------------------------------------------------------------

enum GUEST_CODE_BIT_MASKS
{
    MASK_IMM = 0x20,
    MASK_REG = 0x40,
    MASK_MEM = 0x80,
    MASK_CMD = 0x1F,
};

//-----------------------------------------------------------------------------

enum IR_REGS_IDS
{
    IR_RAX_ID = 1,
    IR_RBX_ID = 2,
    IR_RCX_ID = 3,
    IR_RDX_ID = 4,
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

//-----------------------------------------------------------------------------

typedef union Address
{
    int   guest;
    char *x64;
} Address;

//-----------------------------------------------------------------------------

typedef struct IR_node
{
    uint32_t command;
    Imm_val  imm_val;
    Address  address;
    uint8_t  reg_num;
    uint32_t is_mem_access : 1;
} IR_node;

//-----------------------------------------------------------------------------

typedef struct IR
{
    IR_node *buffer;
    int      capacity;
    int      size;
} IR;

//-----------------------------------------------------------------------------

Guest_code *readGuestFile (FILE *code_file, FILE *log_file);

void parseGuestCode (Guest_code *guest_code, IR *ir, FILE *log_file);

void translateGuestJmpTargets (IR *ir, FILE *log_file);

void searchForTarget (IR *ir, IR_node *ir_node, FILE *log_file);

int translateInstrToIr (IR_node *ir_node, Guest_code *bin_code, int curr_pos);

void irDump (IR *ir, FILE *log_file);

IR *irCtor (FILE *log_file);

void irResize (IR *ir, FILE *log_file);

void irDtor (IR *ir, FILE *log_file);

void guestCodeDtor (Guest_code *bin_code);

//-----------------------------------------------------------------------------

#endif //IR_PARSER_H