#include "CPU/include/CPU.h"

//-----------------------------------------------------------------------------

int processor_ctor (Processor *cpu)
{
    cpu->info = { 0 };

    cpu->Stk = { 0 };
    stack_ctor (&cpu->Stk, SIZE_OF_STK);

    cpu->Stk_call = { 0 };
    stack_ctor (&cpu->Stk_call, SIZE_OF_STK);

    cpu->regs = (double*) calloc (N(REGS), sizeof (double));
    cpu->ram  = (double*) calloc (SIZE_OF_RAM, sizeof (double));

    if(cpu->regs == NULL || cpu->ram == NULL)
    {
        return ERROR_CTOR;
    }

    return (cpu_info_ctor (&cpu->info));
}

//-----------------------------------------------------------------------------

int cpu_info_ctor (Cpu_info *info)
{
    info->code_file = fopen ("processor/COMMON/files/code.bin", "rb");
    info->file_out  = fopen ("processor/CPU/dump/log.txt",      "w+");

    if(info->code_file == NULL ||
       info->file_out  == NULL   )
    {
        return ERROR_CTOR;
    }

    return 0;
}

//-----------------------------------------------------------------------------

void processor_dtor (Processor *cpu)
{
    stack_dtor (&cpu->Stk);
    stack_dtor (&cpu->Stk_call);

    free (cpu->regs);
    free (cpu->ram);
    free (cpu->code);

    cpu_info_dtor (&cpu->info);
}

//-----------------------------------------------------------------------------

void cpu_info_dtor (Cpu_info *info)
{
    fclose (info->code_file);
    fclose (info->file_out);
}

//-----------------------------------------------------------------------------

void read_code_file (Processor *cpu)
{
    double code_signature = 0;
    double res_sum        = 0;

    fread (&res_sum,        sizeof(char), O(ARG), cpu->info.code_file);
    fread (&code_signature, sizeof(char), O(ARG), cpu->info.code_file);

    cpu->code_size = res_sum;

    if(code_signature == SIGNATURE)
    {
        cpu->code = (char*) calloc (cpu->code_size, sizeof (char));

        *(elem_t*)(cpu->code) = res_sum;
        *(elem_t*)(cpu->code + O(ARG)) = code_signature;

        if(cpu->code == NULL)
        {
            printf ("__________|ERROR - NULL pointer code|__________\n");
        }

        fread (cpu->code + O(CODE_SIGNATURE), sizeof(char), cpu->code_size - O(CODE_SIGNATURE), cpu->info.code_file);
    }

    else
    {
        printf ("__________|WRONG SIGNATURE!|__________\n");
    }
}

//-----------------------------------------------------------------------------

void handle_cmds (Processor *cpu)
{
    for(int curr_pos = 2 * O(ARG); curr_pos < cpu->code_size; curr_pos++)
    {
        int     curr_cmd   = cpu->code[curr_pos];
        int     offset     = 0;
        double  pop_value  = 0;
        double  arg_value  = 0;
        double *curr_arg   = &pop_value;

        if(curr_cmd & MASK_REG)
        {
            curr_arg = cpu->regs + (int) *(elem_t*)(cpu->code + curr_pos + O(CMD));
            arg_value += *curr_arg;

            offset += O(ARG);
        }

        if(curr_cmd & MASK_IMM)
        {
            curr_arg = (elem_t*)(cpu->code + curr_pos + offset + O(CMD));
            arg_value += *curr_arg;

            offset += O(ARG);
        }

        if(curr_cmd & MASK_RAM)
        {
            curr_arg = cpu->ram + (int) arg_value;
            arg_value = *curr_arg;
        }

        curr_pos += offset;

        execute_cmd (curr_cmd, curr_arg, arg_value, &curr_pos, cpu);

        if(cpu->is_stop)
        {
            break;
        }
    }
}

//-----------------------------------------------------------------------------

bool is_equal (double a, double b)
{
    const double EPS = 1e-1;

    return (a - b < EPS && a - b > -EPS);
}

//-----------------------------------------------------------------------------

void execute_cmd (int curr_cmd,  double    *curr_arg, double arg_value,
                  int *curr_ptr, Processor *cpu                        )
{
    int curr_pos = *curr_ptr;

    curr_cmd &= MASK_CMD;

    #define CMD_DEF(cmd, name, code, ...) \
        case cmd:                         \
        {                                 \
            code                          \
            __VA_ARGS__                   \
            break;                        \
        }

    switch(curr_cmd)
    {
        #include "COMMON/include/codegen/codegen.h"

        default:
            printf ("?%d \n", curr_cmd);
            break;
    }

    #undef CMD_DEF

    stack_dumps (&cpu->Stk, cpu->info.file_out);

    *curr_ptr = curr_pos;
}

//-----------------------------------------------------------------------------

void cpu_dump (Processor *cpu)
{
    FILE *code_dmp_file = fopen ("processor/CPU/dump/code_cpu_dump.txt", "w+");

    fprintf (code_dmp_file, "%d - size, %x - signature\n", (int) *(elem_t*)(cpu->code + 0),
                                                           (int) *(elem_t*)(cpu->code + O(ARG)));
    int i = 1;

    for(int curr_pos = O(CODE_SIGNATURE); curr_pos < cpu->code_size; curr_pos++)
    {
        char curr_cmd = cpu->code[curr_pos];
        int  offset   = 0;

        fprintf (code_dmp_file, "%06d || %d\n", i++, (int) cpu->code[curr_pos]);

        if(curr_cmd & MASK_REG)
        {
            fprintf (code_dmp_file, "%06d || %lg\n", i++, *(elem_t*)(cpu->code + curr_pos + O(CMD)));

            offset += O(ARG);
        }

        if(curr_cmd & MASK_IMM)
        {
            fprintf (code_dmp_file, "%06d || %lg\n", i++, *(elem_t*)(cpu->code + curr_pos + O(CMD) + offset));

            offset += O(ARG);
        }

        curr_pos += offset;
    }

    fclose (code_dmp_file);
}

//-----------------------------------------------------------------------------
