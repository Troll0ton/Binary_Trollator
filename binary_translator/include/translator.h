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

#define writeSimpleCode(value)                       \
    writeCode_(x64_code, value, #value, value##_SIZE)


#define writePtr(value)                                   \
    writeCode_(x64_code, value, "PTR 4 BYTE", SIZE_OF_PTR)


#define writeAbsPtr(value)                                        \ 
    writeCode_(x64_code, value, "ABS PTR 8 BYTE", SIZE_OF_ABS_PTR)


#define writeNum(value)                                                   \
    writeCode_(x64_code, *(uint64_t*) &(value), "NUM 8 BYTE", SIZE_OF_NUM)


#define writeMaskingCode(OP_MASK, REG_MASK, POS_MASK_REG, MASK_R)     \                                                         
    writeCode_(x64_code, OP_MASK | REG_MASK << POS_MASK_REG | MASK_R, \
               #OP_MASK " " reg_name[REG_MASK], OP_MASK##_SIZE)

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
    X64_CODE_INCREASE_PAR = 2,
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

static char opname[100] = { 0 };

static const char *reg_name[] =
{
    "RAX",
    "RCX",
    "RDX",
    "RBX",
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

void *alignedCalloc(int alignment, int size);

void translateCmd (X64_code *x64_code, IR_node *curr_node);

void translateReg (IR_node *curr_node);

void translateHlt (X64_code *x64_code, IR_node *curr_node);

void translatePush (X64_code *x64_code, IR_node *curr_node);

void translatePop (X64_code *x64_code, IR_node *curr_node);

void translateArithmOperations (X64_code *x64_code, IR_node *curr_node);

void translateStdio (X64_code *x64_code, IR_node *curr_node);

void translateDump (X64_code *x64_code, IR_node *curr_node);

void translateConditionalJmps (X64_code *x64_code, IR_node *curr_node);

void translateJmp (X64_code *x64_code, IR_node *curr_node);

void translateCall(X64_code *x64_code, IR_node *curr_node);

void translateRet(X64_code *x64_code, IR_node *curr_node);

void translateMathFunctions (X64_code *x64_code, IR_node *curr_node);

void writeCode_(X64_code *x64_code, uint64_t value, char *name, int size);

void dumpCode (X64_code *x64_code, char *name, int size);

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