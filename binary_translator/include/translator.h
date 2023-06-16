//! @file translator.h

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/x64_codes.h"
#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

//-----------------------------------------------------------------------------

#define CURR_POS x64_code->curr_pos

//-----------------------------------------------------------------------------

#define MASKING(OP_MASK, REG_MASK, POS_MASK_REG, MASK_R) \
    OP_MASK | REG_MASK << POS_MASK_REG | MASK_R, #OP_MASK " " #OP_MASK

//-----------------------------------------------------------------------------
//                                WRITING
//-----------------------------------------------------------------------------

#define writeSimpleOp(name)                       \
    writeCode_(x64_code, name, #name, name##_SIZE)


#define writeByte(value)                     \
    writeCode_(x64_code, value, "BYTE", BYTE)


#define writePtr(value)                                   \
    writeCode_(x64_code, value, "PTR 4 BYTE", SIZE_OF_PTR)


#define writeAbsPtr(value)                                        \
    writeCode_(x64_code, value, "ABS PTR 8 BYTE", SIZE_OF_ABS_PTR)


#define writeDouble(value)                                          \
    writeCode_(x64_code, *(uint64_t*) &(value), "DOUBLE 8 BYTE", 8)


#define writeInt64(value)                        \
    writeCode_(x64_code, value, "INT 8 BYTE", 8)


#define writeInt32(value)                        \
    writeCode_(x64_code, value, "INT 8 BYTE", 4)


#define writeMaskingOp(opname, REG)                                                                                \
    strncat (dump_name, #opname " ", 90);                                                                          \
    strncat (dump_name, reg_info[REG].name, 10);                                                                   \
                                                                                                                   \
    writeCode_(x64_code, opname | reg_info[REG].mask << POS_##opname | reg_info[REG].reg_flag << MASK_R_##opname,  \
               dump_name, opname##_SIZE);                                                                          \
                                                                                                                   \
    memset (dump_name, '\0', 100);


#define writeMaskingJmp(JMP)                                                                        \
                                                                                                    \
    writeCode_(x64_code, OP_CONDITIONAL_JMP | JMP << POS_OP_MASK_JMP,                               \
               #JMP, OP_CONDITIONAL_JMP_SIZE);                                                                   

//-----------------------------------------------------------------------------

enum RAM_INFO
{
    RAM_INIT_SIZE = PAGESIZE,
};

//-----------------------------------------------------------------------------

enum X64_CODE_INFO
{
    X64_CODE_INIT_SIZE    = PAGESIZE,
    X64_CODE_SIZE_DIFF    = 16,
    X64_CODE_INCREASE_PAR = PAGESIZE,
};

//-----------------------------------------------------------------------------

enum X64_ARCHITECTURE_INFO
{
    BYTE            = 1,
    SIZE_OF_PTR     = 4 * BYTE,       // DOUBLE WORD
    SIZE_OF_ABS_PTR = sizeof (void*),
    SIZE_OF_NUM     = sizeof (double),
};

//-----------------------------------------------------------------------------

enum COMMON_JMPS_OFFSETS
{
    X64_CONDITIONAL_JUMP_OFFSET = 20,
    X64_JUMP_OFFSET             = 1,
    X64_CALL_OFFSET             = 15,
}; 

//-----------------------------------------------------------------------------
// Info wasn't used here because of smallness of amount of fields (only dump file)
typedef struct X64_code
{
    char *buffer;
    int   size;
    char *curr_pos;
    int   capacity;
    FILE *dump_file;
} X64_code;

//-----------------------------------------------------------------------------

typedef struct Ram
{
    char *buffer;
    int   size;
} Ram;

//-----------------------------------------------------------------------------

typedef struct Jmp_table
{
    char **buffer;
    int    size;
    FILE  *dump_file;
} Jmp_table;   

//-----------------------------------------------------------------------------
//                         REGS MASKING AND DUMPS UTILS
//-----------------------------------------------------------------------------

static char dump_name[100] = { 0 };

typedef struct Reg_info
{
    const char *name;
    char        reg_flag;
    uint64_t    mask;
} Reg_info;

static const Reg_info reg_info[] =
{
    {"RAX", 0, MASK_RAX},
    {"RCX", 0, MASK_RCX},
    {"RDX", 0, MASK_RDX},
    {"RBX", 0, MASK_RBX},
    {"RSP", 0, MASK_RSP},
    {"RBP", 0, MASK_RBP},
    {"RSI", 0, MASK_RSI},
    {"RDI", 0, MASK_RDI},

    {"R8",  1, MASK_R8  - 0b1000},
    {"R9",  1, MASK_R9  - 0b1000},
    {"R10", 1, MASK_R10 - 0b1000},
    {"R11", 1, MASK_R11 - 0b1000},
    {"R12", 1, MASK_R12 - 0b1000},
    {"R13", 1, MASK_R13 - 0b1000},
    {"R14", 1, MASK_R14 - 0b1000},
    {"R15", 1, MASK_R15 - 0b1000},
};

//-----------------------------------------------------------------------------

X64_code *translateIrToX64(IR *ir, int bin_size);

X64_code *x64CodeCtor (int init_size, int alignment);

void x64CodeDtor (X64_code *x64_code);

Ram *ramCtor (int size, int alignment);

void ramDtor (Ram *ram);

Jmp_table *jmpTableCtor (int size);

void jmpTableDtor (Jmp_table *jmp_table);

void handleJmpTargetsX64 (X64_code *x64_code, IR *ir, Jmp_table *jmp_table);

void translateTargetPtr (X64_code *x64_code, IR_node ir_node, Jmp_table *jmp_table);

void *alignedCalloc (int alignment, int size);

void translateCmd (X64_code *x64_code, IR_node *curr_node);

void translateReg (IR_node *curr_node);

void translateHlt (X64_code *x64_code, IR_node *curr_node);

void calculateRamAddrPushPop (X64_code *x64_code, IR_node *curr_node);

void translatePush (X64_code *x64_code, IR_node *curr_node);

void translatePushRam (X64_code *x64_code, IR_node *curr_node);

void translatePushRegImm (X64_code *x64_code, IR_node *curr_node);

void translatePop (X64_code *x64_code, IR_node *curr_node);

void translatePopRam (X64_code *x64_code, IR_node *curr_node);

void translatePopReg (X64_code *x64_code, IR_node *curr_node);

void translateArithmOperations (X64_code *x64_code, IR_node *curr_node);

void translateStdio (X64_code *x64_code, IR_node *curr_node);

void translateDump (X64_code *x64_code, IR_node *curr_node);

void translateConditionalJmps (X64_code *x64_code, IR_node *curr_node);

void translateJmp (X64_code *x64_code, IR_node *curr_node);

void translateCall(X64_code *x64_code, IR_node *curr_node);

void translateRet(X64_code *x64_code, IR_node *curr_node);

void translateMathFunctions (X64_code *x64_code, IR_node *curr_node);

void writeCode_(X64_code *x64_code, uint64_t value, const char *name, int size);

void dumpCode (X64_code *x64_code, const char *name, int size);

void writePrologue (X64_code *x64_code);

void writeEpilogue (X64_code *x64_code);

void saveDataAddress (X64_code *x64_code, char *ram);

void double_scanf (double *value);

void double_printf (double *value);

void runCode (char *code, int size);

void CodeX64DumpHeader (X64_code *x64_code);

void jmpTableDump (Jmp_table *jmp_table);

//-----------------------------------------------------------------------------

#endif // TRANSLATOR_H