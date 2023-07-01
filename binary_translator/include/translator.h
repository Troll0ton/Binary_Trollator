//! @file translator.h

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/x64_codes.h"
#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/common.h"
#include "binary_translator/include/input_output.h" 

//-----------------------------------------------------------------------------

#define CURR_POS x64_code->curr_pos

enum ELF_INFO
{
    NUM_OF_SEGMENTS = 3,
    LOAD_ADDR   = 0x400000,
    TEXT_ADDR   = LOAD_ADDR + sizeof (Elf64_Ehdr) + 
                  NUM_OF_SEGMENTS * sizeof (Elf64_Phdr),
    // ??                
    RAM_ADDR    = TEXT_ADDR + PAGE_SIZE, 
    FUNCT_ADDR  = RAM_ADDR  + PAGE_SIZE, 
    TOTAL_SIZE  = sizeof (Elf64_Ehdr) + 
                  NUM_OF_SEGMENTS * (sizeof (Elf64_Phdr) + PAGE_SIZE),
};


//-----------------------------------------------------------------------------

#define MEMORY_SIZE (3 * PAGE_SIZE)

//-----------------------------------------------------------------------------
//                                WRITING
//-----------------------------------------------------------------------------

#define writeSimpleOp(name)                                 \
    writeCode_(x64_code, name, #name, name##_SIZE, log_file)


#define writeByte(value)                                \
    writeCode_(x64_code, value, "BYTE", BYTE, log_file)


#define writePtr(value)                                                 \
    writeCode_(x64_code, value, "PTR 4 BYTE", SIZE_OF_PTR, log_file)


#define writeAbsPtr(value)                                                      \
    writeCode_(x64_code, value, "ABS PTR 8 BYTE", SIZE_OF_ABS_PTR, log_file)


#define writeDouble(value)                                                      \
    writeCode_(x64_code, *(uint64_t*) &(value), "DOUBLE 8 BYTE", 8, log_file)


#define writeInt64(value)                                   \
    writeCode_(x64_code, value, "INT 8 BYTE", 8, log_file)


#define writeInt32(value)                                   \
    writeCode_(x64_code, value, "INT 4 BYTE", 4, log_file)


#define writeMaskingOp(opname, REG)                                                                                \
    strncat (dump_name, #opname " ", 90);                                                                          \
    strncat (dump_name, reg_info[REG].name, 10);                                                                   \
                                                                                                                   \
    writeCode_(x64_code, opname | reg_info[REG].mask << POS_##opname | reg_info[REG].reg_flag << MASK_R_##opname,  \
               dump_name, opname##_SIZE, log_file);                                                                \
                                                                                                                   \
    memset (dump_name, '\0', 100);


#define writeMaskingJmp(JMP)                                          \
    writeCode_(x64_code, OP_CONDITIONAL_JMP | JMP << POS_OP_MASK_JMP, \
               #JMP, OP_CONDITIONAL_JMP_SIZE, log_file);   


#define writeFormatStr(name)                       \
    writeCode_(x64_code, name, #name, name##_SIZE, log_file)                                                                

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

enum X64_CODE_INFO
{
    X64_CODE_SIZE_DIFF            = 16,
    X64_CODE_INIT_SIZE            = PAGE_SIZE,
    X64_CODE_INCREASE_PAR         = PAGE_SIZE,
    X64_CODE_REG_MASK             = 0b1000,
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

// This enum contains target's position (relative address from beginning of 
// given IR node)
enum JUMP_TARGETS_POS
{
    POS_CONDITIONAL_JUMP        = OP_MOV_XMM0_STK_SIZE + 
                                  OP_MOV_XMM1_STK_SIZE + 
                                  OP_ADD_REG_IMM_SIZE +
                                  4 +
                                  OP_CMP_XMM0_XMM1_SIZE +
                                  OP_CONDITIONAL_JMP_SIZE,

    POS_JUMP                    = OP_JMP_SIZE,

    POS_CALL                    = OP_CALL_SIZE,

    POS_IN                      = OP_SUB_REG_IMM_SIZE + 
                                  4 +
                                  OP_LEA_RDI_STK_ARG_SIZE + 
                                  OP_PUSHA_SIZE +
                                  OP_PUSH_REG_SIZE + 
                                  OP_MOV_RBP_RSP_SIZE +
                                  OP_ALIGN_STK_SIZE + 
                                  OP_SUB_REG_IMM_SIZE +
                                  4 +
                                  OP_PUSH_REG_SIZE +
                                  OP_CALL_SIZE,
    #ifdef ELF_MODE
    POS_OUT                     = OP_MOV_XMM0_STK_SIZE + 
                                  OP_PUSH_REG_SIZE + 
                                  OP_CVTTSD2SI_REG_SIZE +
                                  OP_PUSHA_SIZE +
                                  OP_PUSH_REG_SIZE + 
                                  OP_MOV_RBP_RSP_SIZE +
                                  OP_ALIGN_STK_SIZE + 
                                  OP_SUB_REG_IMM_SIZE +
                                  4 +
                                  OP_PUSH_REG_SIZE +
                                  OP_CALL_SIZE,
    #else
    POS_OUT                     = OP_LEA_RDI_STK_ARG_SIZE + 
                                  OP_PUSHA_SIZE +
                                  OP_PUSH_REG_SIZE + 
                                  OP_MOV_RBP_RSP_SIZE +
                                  OP_ALIGN_STK_SIZE + 
                                  OP_SUB_REG_IMM_SIZE +
                                  4 +
                                  OP_PUSH_REG_SIZE +
                                  OP_CALL_SIZE,
    #endif
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
//                         REGS MASKING AND DUMPS UTILS
//-----------------------------------------------------------------------------
// ??
static char dump_name[100] = { 0 };

// another header file

typedef struct Reg_info
{
    const char *name;
    char        reg_flag;
    uint64_t    mask;
} Reg_info;

// make functons 

// This array allow us to use more codegeneration in writeCode function
// Register's masks are similar in different opcodes but there is a nuance here
// For example: Difference between push rax, r8:
//  push rax | 40 50
//  push r8  | 41 50
// To select correct opcode I need to set register's bit
// In common occassion opcodes of some instructions which includes rax, rbx ...
// are shorter than r8, r9 ...
// But in way to follow standart I desided to use similar sizes of these opcodes.
static const Reg_info reg_info[] =
{
    //  register's bit
    //      |
    //      V
    {"RAX", 0, MASK_RAX},
    {"RCX", 0, MASK_RCX},
    {"RDX", 0, MASK_RDX},
    {"RBX", 0, MASK_RBX},
    {"RSP", 0, MASK_RSP},
    {"RBP", 0, MASK_RBP},
    {"RSI", 0, MASK_RSI},
    {"RDI", 0, MASK_RDI},
    {"R8",  1, MASK_R8  - X64_CODE_REG_MASK},
    {"R9",  1, MASK_R9  - X64_CODE_REG_MASK},
    {"R10", 1, MASK_R10 - X64_CODE_REG_MASK},
    {"R11", 1, MASK_R11 - X64_CODE_REG_MASK},
    {"R12", 1, MASK_R12 - X64_CODE_REG_MASK},
    {"R13", 1, MASK_R13 - X64_CODE_REG_MASK},
    {"R14", 1, MASK_R14 - X64_CODE_REG_MASK},
    {"R15", 1, MASK_R15 - X64_CODE_REG_MASK},
};

//-----------------------------------------------------------------------------

X64_code *translateIrToX64 (IR *ir, int bin_size, FILE *log_file);

X64_code *x64CodeCtor (int init_size, int alignment, FILE *log_file);

void x64CodeDtor (X64_code *X64_code);

Memory *memoryCtor (int size, int alignment, FILE *log_file);

void memoryDtor (Memory *ram);

void handleX64JmpTargets (X64_code *x64_code, IR *ir, FILE *log_file);

void handleIOAddress (X64_code *x64_code, IR_node ir_node, FILE *log_file);

void translateTargetPtr (X64_code *x64_code, IR *ir, IR_node ir_node, FILE *log_file);

void *alignedCalloc (int alignment, int size, FILE *log_file);

void translateCmd (X64_code *X64_code, IR_node *curr_node, FILE *log_file);

void translateReg (IR_node *curr_node, FILE *log_file);

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

void writePrologue (X64_code *X64_code, FILE *log_file);

void writeEpilogue (X64_code *X64_code, FILE *log_file);

void saveDataAddress (X64_code *x64_code, char *ram, FILE *log_file);

void doubleScanf (double *value);

void doublePrintf (double *value);

void x64DumpHeader (X64_code *X64_code, FILE *log_file);

//-----------------------------------------------------------------------------

#endif // TRANSLATOR_H