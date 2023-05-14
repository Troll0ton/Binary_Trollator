#include "binary_translator/include/IR-parser.h" 

//-----------------------------------------------------------------------------

Bin_code *readCodeFile (FILE *code_file)
{
    Bin_code *bin_code = (Bin_code*) calloc (1, sizeof (Bin_code));

    long long int code_signature = 0;
    long long int res_sum        = 0;

    fread (&res_sum,        sizeof (signature), 1, code_file);
    fread (&code_signature, sizeof (signature), 1, code_file);

    bin_code->size = res_sum;

    if(code_signature == SIGNATURE)
    {
        bin_code->buffer = (char*) calloc (bin_code->size, sizeof (char));

        ((long long int*) bin_code->buffer)[0] = res_sum;
        ((long long int*) bin_code->buffer)[1] = code_signature;

        if(bin_code->buffer == NULL)
        {
            printf ("__________|ERROR - NULL pointer code|__________\n");
        }

        fread (bin_code->buffer + OFFSET_CODE_SIGNATURE, 
               sizeof(char), bin_code->size - OFFSET_CODE_SIGNATURE, 
               code_file);

        return bin_code;
    }

    printf ("__________|WRONG SIGNATURE!|__________\n");
    free (bin_code);

    return NULL;
}

//-----------------------------------------------------------------------------

void translateBinToIR (Processor *cpu)
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

    *curr_ptr = curr_pos;
}

//-----------------------------------------------------------------------------