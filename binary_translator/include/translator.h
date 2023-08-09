//! @file translator.h

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/x64_codes.h"
#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/common.h"
#include "binary_translator/include/input_output.h" 

//-----------------------------------------------------------------------------

#define MAX_LEN_OF_LINE 100

//-----------------------------------------------------------------------------
//                                WRITING
//-----------------------------------------------------------------------------

// do { } while ( 0 )
// if () makeRegMask()
// magic constant
#define makeRegMask(cmd, reg)                         \
    makeRegMask_(reg, cmd##_ID_POS, cmd##_REX_B_POS); \
    strncat (op_name, reg_name[reg], 30);
                                                        

#define writeOpcode(cmd, mask)                                                \
    strncat (op_name, #cmd, 70);                                              \
    writeCode_(x64_code, cmd##_OPCODE | mask, op_name, cmd##_SIZE, log_file); \
    memset (op_name, '\0', MAX_LEN_OF_LINE); 


#define writeValue(value, size)                          \
    writeCode_(x64_code, value, "VALUE", size, log_file);


//-----------------------------------------------------------------------------

static char *reg_name[] = 
{
     "(RAX) ",
     "(RCX) ",
     "(RDX) ",
     "(RBX) ",
     "(RSP) ",
     "(RBP) ",
     "(RSI) ",
     "(RDI) ",
     "(R8) ", 
     "(R9) ",
     "(R10) ",
     "(R11) ",
     "(R12) ",
     "(R13) ",
     "(R14) ",
     "(R15) ",
};

//-----------------------------------------------------------------------------

enum CODE_RESIZE_INFO
{
    X64_CODE_SIZE_DIFF    = 16,
    X64_CODE_INIT_SIZE    = PAGE_SIZE,
    X64_CODE_INCREASE_PAR = PAGE_SIZE,
};

//-----------------------------------------------------------------------------

enum X64_DATA_TYPES
{
    BYTE             = 1,
    // ??
    SIZE_OF_PTR      = 4 * BYTE,                                              
    SIZE_OF_ABS_PTR  = sizeof (void*),
    SIZE_OF_LONG_NUM = sizeof (double),
    SIZE_OF_NUM      = sizeof (int),
};

//-----------------------------------------------------------------------------

typedef struct X64_code
{
    char *buffer;
    int   size;
    char *curr_pos;
    int   capacity;
    FILE *dump_file;
} X64_code;

//-----------------------------------------------------------------------------

typedef struct Memory
{
    char *buffer;
    int   size;
} Memory;

//-----------------------------------------------------------------------------

X64_code *translateBinCode (char *guest_code_file_name, FILE *log_file);

X64_code *translateIrToX64 (IR *ir, int bin_size, FILE *log_file);

X64_code *x64CodeCtor (int init_size, int alignment, FILE *log_file);

void x64CodeDtor (X64_code *X64_code);

Memory *memoryCtor (int size, int alignment, FILE *log_file);

void memoryDtor (Memory *memory);

void handleX64JmpTargets (X64_code *x64_code, IR *ir, FILE *log_file);

void translateTargetPtr (X64_code *x64_code, IR *ir, IR_node ir_node, FILE *log_file);

void *alignedCalloc (int alignment, int size, FILE *log_file);

void translateCmd (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateReg (IR_node *curr_node, FILE *log_file);

uint64_t makeRegMask_(int reg_id, int reg_id_pos, int rex_b_pos);

uint64_t makeJmpMask (int jmp_type);

void translateHlt (X64_code *x64_code, IR_node *curr_node, FILE *log_file);

void calculateMemoryAddrPushPop (X64_code *x64_code, 
                                 IR_node  *curr_node, 
                                 FILE     *log_file  );

void translatePush (X64_code *x64_code, IR_node *curr_node, FILE *log_file);

void translatePushMemory (X64_code *x64_code, IR_node *curr_node, FILE *log_file);

void translatePushRegImm (X64_code *x64_code, IR_node *curr_node, FILE *log_file);

void translatePop (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translatePopMemory (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translatePopReg (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateArithmOperations (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateOut (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateIn (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateDump (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateConditionalJmps (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void x64CodeResize (X64_code *x64_code, FILE *log_file);

void translateJmp (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateCall (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateRet (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateMathFunctions (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void writeCode_(X64_code *x64_code, uint64_t value, const char *name, int size, FILE *log_file);

void dumpCode (X64_code *X64_code, const char *name, int size, FILE *log_file);

void saveCtx (X64_code *X64_code, FILE *log_file);

void loadCtx (X64_code *X64_code, FILE *log_file);

void saveDataAddress (X64_code *x64_code, char *memory, FILE *log_file);

void doubleScanf (double *value);

void doublePrintf (double *value);

void x64DumpHeader (X64_code *X64_code, FILE *log_file);

//-----------------------------------------------------------------------------

#endif // TRANSLATOR_H