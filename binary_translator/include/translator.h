//! @file translator.h

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/x64_codes.h"
#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/common.h"
#include "binary_translator/include/input_output.h" 

//-----------------------------------------------------------------------------

#define CURR_POS host_code->curr_pos

//-----------------------------------------------------------------------------
//                                WRITING
//-----------------------------------------------------------------------------

#define writeSimpleOp(name)                       \
    writeCode_(host_code, name, #name, name##_SIZE)


#define writeByte(value)                     \
    writeCode_(host_code, value, "BYTE", BYTE)


#define writePtr(value)                                   \
    writeCode_(host_code, value, "PTR 4 BYTE", SIZE_OF_PTR)


#define writeAbsPtr(value)                                        \
    writeCode_(host_code, value, "ABS PTR 8 BYTE", SIZE_OF_ABS_PTR)


#define writeDouble(value)                                          \
    writeCode_(host_code, *(uint64_t*) &(value), "DOUBLE 8 BYTE", 8)


#define writeInt64(value)                        \
    writeCode_(host_code, value, "INT 8 BYTE", 8)


#define writeInt32(value)                        \
    writeCode_(host_code, value, "INT 4 BYTE", 4)


#define writeMaskingOp(opname, REG)                                                                                \
    strncat (dump_name, #opname " ", 90);                                                                          \
    strncat (dump_name, reg_info[REG].name, 10);                                                                   \
                                                                                                                   \
    writeCode_(host_code, opname | reg_info[REG].mask << POS_##opname | reg_info[REG].reg_flag << MASK_R_##opname,  \
               dump_name, opname##_SIZE);                                                                          \
                                                                                                                   \
    memset (dump_name, '\0', 100);


#define writeMaskingJmp(JMP)                                          \
    writeCode_(host_code, OP_CONDITIONAL_JMP | JMP << POS_OP_MASK_JMP, \
               #JMP, OP_CONDITIONAL_JMP_SIZE);   


#define writeFormatStr(name)                       \
    writeCode_(host_code, name, #name, name##_SIZE)                                                                

//-----------------------------------------------------------------------------

// EsecutableInfo
enum RAM_INFO
{
    RAM_INIT_SIZE = PAGE_SIZE,
};

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

typedef struct Host_code
{
    char *buffer;
    int   size;
    char *curr_pos;
    int   capacity;
    FILE *dump_file;
} Host_code;

//-----------------------------------------------------------------------------

// ??
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

Host_code *translateIrToHost (IR *ir, int bin_size);

Host_code *hostCodeCtor (int init_size, int alignment);

void hostCodeDtor (Host_code *Host_code);

Ram *ramCtor (int size, int alignment);

void ramDtor (Ram *ram);

Jmp_table *jmpTableCtor (int size);

void jmpTableDtor (Jmp_table *jmp_table);

void handleHostJmpTargets (Host_code *Host_code, IR *ir, Jmp_table *jmp_table, 
                          char *in_addr, char *out_addr                    );

void handleIOAddress (Host_code *Host_code, IR_node ir_node, 
                      char *in_addr, char *out_addr       );

void translateTargetPtr (Host_code *Host_code, IR_node ir_node, Jmp_table *jmp_table);

void *alignedCalloc (int alignment, int size);

void translateCmd (Host_code *Host_code, IR_node *curr_node);

void translateReg (IR_node *curr_node);

void translateHlt (Host_code *Host_code, IR_node *curr_node);

void calculateRamAddrPushPop (Host_code *Host_code, IR_node *curr_node);

void translatePush (Host_code *Host_code, IR_node *curr_node);

void translatePushRam (Host_code *Host_code, IR_node *curr_node);

void translatePushRegImm (Host_code *Host_code, IR_node *curr_node);

void translatePop (Host_code *Host_code, IR_node *curr_node);

void translatePopRam (Host_code *Host_code, IR_node *curr_node);

void translatePopReg (Host_code *Host_code, IR_node *curr_node);

void translateArithmOperations (Host_code *Host_code, IR_node *curr_node);

void translateOut (Host_code *Host_code, IR_node *curr_node);

void translateIn (Host_code *Host_code, IR_node *curr_node);

void translateDump (Host_code *Host_code, IR_node *curr_node);

void translateConditionalJmps (Host_code *Host_code, IR_node *curr_node);

void translateJmp (Host_code *Host_code, IR_node *curr_node);

void translateCall (Host_code *Host_code, IR_node *curr_node);

void translateRet (Host_code *Host_code, IR_node *curr_node);

void translateMathFunctions (Host_code *Host_code, IR_node *curr_node);

void writeCode_(Host_code *Host_code, uint64_t value, const char *name, int size);

void dumpCode (Host_code *Host_code, const char *name, int size);

void writePrologue (Host_code *Host_code);

void writeEpilogue (Host_code *Host_code);

void saveDataAddress (Host_code *Host_code, char *ram);

void double_scanf (double *value);

void double_printf (double *value);

void runCode (char *code, int size);

void hostDumpHeader (Host_code *Host_code);

void jmpTableDump (Jmp_table *jmp_table);

//-----------------------------------------------------------------------------

#endif // TRANSLATOR_H