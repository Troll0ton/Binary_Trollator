//! @file translator.h

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/x64_codes.h"
#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

static const uint32_t tmp_size = 20;
static const int max_len_dump = 15;

//-----------------------------------------------------------------------------

enum TRANSLATOR_INFO
{
    ABSENCE        = 0xBEDBAD,
    DELETED        = 0xDEDAC,
    MPROTECT_ERROR = -1,
};

typedef enum RAM_INFO
{
    RAM_INIT_SIZE = PAGESIZE,
} RAM_INFO;

typedef enum X64_CODE_INFO
{
    X64_CODE_INIT_SIZE    = PAGESIZE,
    X64_CODE_SIZE_DIFF    = 16,
    X64_CODE_INCREASE_PAR = 2,
} X64_CODE_INFO;

enum X64_INFO
{
    SIZE_OF_PTR = 4, // DOUBLE WORD
    SIZE_OF_ABS_PTR = sizeof (void*),
    SIZE_OF_NUM = sizeof (double),

    X64_ADD_R13_R_X_OFFSET = 18,
    X64_CONDITIONAL_JUMP_OFFSET = 20,
    X64_JUMP_OFFSET = 1,
    X64_CALL_OFFSET = 15,
};

enum MASK_INFO
{
    JMP_MASK_LEN = 8,
    X64_MOV_R_X_STK_MASK_LEN = 18,
};

//-----------------------------------------------------------------------------

typedef struct X64_code
{
    char *buffer;
    int   size;
    char *curr_pos;
    int   capacity;
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
} Jmp_table;   

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

void translateCmd(IR_node *curr_node, char **curr_pos);

void translateHlt(IR_node *curr_node, char **curr_pos);

void translatePush(IR_node *curr_node, char **curr_pos);

void translatePop(IR_node *curr_node, char **curr_pos);

void translateArithmOperations(IR_node *curr_node, char **curr_pos);

void translateStdio(IR_node *curr_node, char **curr_pos);

void translateDump(IR_node *curr_node, char **curr_pos);

void translateConditionalJmps(IR_node *curr_node, char **curr_pos);

void translateJmp(IR_node *curr_node, char **curr_pos);

void translateCall(IR_node *curr_node, char **curr_pos);

void translateRet(IR_node *curr_node, char **curr_pos);

void translateMathFunctions(IR_node *curr_node, char **curr_pos);

void writeCode (X64_code *x64_code, uint64_t value, int size);

void writeDouble (X64_code *x64_code, double num);

void writePrologue (X64_code *x64_code);

void writeEpilogue (X64_code *x64_code);

void saveDataAddress (X64_code *x64_code, char *ram);

void double_scanf(double *value);

void double_printf(double *value);

void runCode (char *code, int size);

void CodeX64Dump (char *code, int size);

void jmpTableDump (Jmp_table *jmp_table);

//-----------------------------------------------------------------------------

#endif // TRANSLATOR_H