//! @file CPU.h

#ifndef   CPU_H
#define   CPU_H

//-----------------------------------------------------------------------------

#include "CPU/include/stack.h"
#include "COMMON/include/common.h"
#include <math.h>

//-----------------------------------------------------------------------------

#define Struct Processor
#define Name   cpu

//-----------------------------------------------------------------------------

enum CPU_INFO
{
    SIZE_OF_STK = 2,
};

//-----------------------------------------------------------------------------

enum REG_INFO
{
    NUM_OF_REGS = 100,
};

//-----------------------------------------------------------------------------

enum RAM_INFO
{
    SIZE_OF_RAM = 900,
};

//-----------------------------------------------------------------------------

typedef struct Cpu_info
{
    FILE *code_file;
    FILE *file_out;
} Cpu_info;

//-----------------------------------------------------------------------------

typedef struct Processor
{
    Cpu_info info;
    double  *regs;
    double  *ram;
    char    *code;
    bool     is_stop;
    int      code_size;
    Stack    Stk;
    Stack    Stk_call;
} Processor;

//-----------------------------------------------------------------------------

int  processor_ctor  (Processor *cpu);

int  cpu_info_ctor   (Cpu_info *info);

void processor_dtor  (Processor *cpu);

void cpu_info_dtor   (Cpu_info *info);

void read_code_file  (Processor *cpu);

void handle_cmds     (Processor *cpu);

bool is_equal        (double a, double b);

void execute_cmd     (int curr_cmd,  double    *curr_arg, double arg_value,
                      int *curr_ptr, Processor *cpu                        );

void cpu_dump        (Processor *cpu);

//-----------------------------------------------------------------------------

#endif //CPU_H
