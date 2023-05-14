//! @file ASM.h

#ifndef   ASM_H
#define   ASM_H

//-----------------------------------------------------------------------------

#include "ASM/include/lines_handle.h"
#include "ASM/include/input_output.h"
#include "ASM/include/stack.h"
#include "COMMON/include/common.h"

//-----------------------------------------------------------------------------

#define lscan(...) sscanf (Asm->curr_line.begin_line, __VA_ARGS__)

//-----------------------------------------------------------------------------

#define SUCCESS_READ 1
#define EMPTY_LINE   0

//-----------------------------------------------------------------------------

enum CMD_INFO
{
    //{----------------------------------------------------------------------------
    //!
    //}----------------------------------------------------------------------------
                                NUM_OF_SUPPORTED_CMD = 22,
    //{----------------------------------------------------------------------------
    //!
    //}----------------------------------------------------------------------------

    MAX_LEN = 20,
};

//-----------------------------------------------------------------------------

enum LABEL_INFO
{
    LABEL_SIZE         = 800,
    O(LABEL_SIGNATURE) = 1,
    PAR_INCREASE       = 40,
};

//-----------------------------------------------------------------------------

typedef struct Asm_info
{
    FILE   *file_in;
    FILE   *code_file;
    int     code_signature;
    bool    double_pass;
} Asm_info;

//-----------------------------------------------------------------------------

typedef struct Command
{
    bool  is_label;
    int   mask;
    int   code;
} Command;

//-----------------------------------------------------------------------------

typedef struct Argument
{
    bool   is_label;
    double value;
    char   reg_sym;
    int    amount;
} Argument;

//-----------------------------------------------------------------------------

typedef struct Code_array
{
    char *array;
    int   size;
    int   capacity;
    FILE *dmp_file;
} Code_array;

//-----------------------------------------------------------------------------

typedef struct Label_array
{
    int  *array;
    int   size;
    int   capacity;
    FILE *dmp_file;
} Label_array;

//-----------------------------------------------------------------------------

typedef struct Assembler
{
    Asm_info    info;
    Line        curr_line;
    int         curr_pos;
    int         offset;
    Code_array  code;
    Label_array label;
} Assembler;

//-----------------------------------------------------------------------------

int  assembler_ctor (Assembler *Asm, char *argv[]);

int  asm_info_ctor  (Asm_info *info, char *argv[]);

void assembling     (Assembler *Asm);

void parse_text     (Assembler *Asm, Line *Text, File *File_input);

void parse_line     (Assembler *Asm, Command *cmd, Argument *arg);

void parse_label    (Assembler *Asm, Argument *arg);

void search_label   (Assembler *Asm, Argument *arg);

void parse_cmd      (Assembler *Asm, Command *cmd, Argument *arg);

void parse_arg      (Assembler *Asm, Command *cmd, Argument *arg);

void write_in_code  (Assembler *Asm, Command  cmd, Argument arg);

void write_in_arg   (Assembler *Asm, Command  cmd, Argument arg);

void write_in_label (Assembler *Asm, Argument arg);

void assembler_dtor (Assembler *Asm);

void asm_info_dtor  (Asm_info  *info);

void write_res_sums (Assembler *Asm);

void asm_dump       (Assembler *Asm);

//-----------------------------------------------------------------------------

#endif //ASM_H
