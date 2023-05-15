#include "binary_translator/include/IR-parser.h" 
#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

Bin_code *readCodeFile (FILE *code_file)
{
    printf ("-- open the code.bin\n\n");

    Bin_code *bin_code = (Bin_code*) calloc (1, sizeof (Bin_code));

    elem_t code_signature = 0;
    elem_t res_sum        = 0;

    fread (&res_sum,        sizeof(char), OFFSET_ARG, code_file);
    fread (&code_signature, sizeof(char), OFFSET_ARG, code_file);

    bin_code->size = res_sum;

    printf ("-------- bin res sum: %d\n\n", bin_code->size);
    printf ("-- code.bin SIGNATURE: %llx\n\n", code_signature);

    if(code_signature == SIGNATURE)
    {
        printf ("-- copy data to array\n\n");

        bin_code->buffer = (char*) calloc (bin_code->size, sizeof (char));

        *(elem_t*)(bin_code->buffer) = res_sum;
        *(elem_t*)(bin_code->buffer + O(ARG)) = code_signature;

        if(bin_code->buffer == NULL)
        {
            printf ("__________|ERROR - NULL pointer code|__________\n\n");
        }

        fread (bin_code->buffer + OFFSET_CODE_SIGNATURE, 
               sizeof(char), 
               bin_code->size - OFFSET_CODE_SIGNATURE, 
               code_file                                );

        printf ("-- successful copying\n\n");

        return bin_code;
    }

    printf ("__________|WRONG SIGNATURE!|__________\n\n");
    free (bin_code);

    return NULL;
}

//-----------------------------------------------------------------------------

#define ir_code intrm_repres->buffer 

Intrm_represent *translateBinToIR (Bin_code *bin_code)
{
    printf ("-- translating to Intermediate Representation\n\n");

    Intrm_represent *intrm_repres = (Intrm_represent*) calloc (1, sizeof (intrm_repres));
    intrm_repres->buffer = (Ir_code*) calloc (bin_code->size, sizeof(Ir_code));

    printf ("-------- ir start size: %d\n\n", bin_code->size);

    handleBinCode (intrm_repres, bin_code);
    /*
    if(intrm_repres->size + 1 < bin_code->size)
    {
        ir_code = (Ir_code*) realloc (ir_code, intrm_repres->size + 1);    
    }
    */
    printf ("-------- ir final size: %d\n\n", intrm_repres->size + 1);
    printf ("-- successful translating\n\n");

    return intrm_repres;
}

//-----------------------------------------------------------------------------

void handleBinCode (Intrm_represent *intrm_repres, Bin_code *bin_code)
{
    int num_cmd = 0;

    for(int curr_pos = 2 * OFFSET_ARG; curr_pos < bin_code->size; curr_pos++)
    {
        int curr_cmd = bin_code->buffer[curr_pos];
        int offset = 0;

        if(curr_cmd & MASK_REG)
        {
            ir_code[num_cmd].reg_num = (int) *(elem_t*)(bin_code->buffer + curr_pos + O(CMD));
            offset += OFFSET_ARG;
        }

        if(curr_cmd & MASK_IMM)
        {
            ir_code[num_cmd].imm_value = (int) *(elem_t*)(bin_code->buffer + curr_pos + offset + O(CMD));
            offset += OFFSET_ARG;
        }

        ir_code[num_cmd].ram_flag = 0;
        
        if(curr_cmd & MASK_RAM)
        {
            ir_code[num_cmd].ram_flag = 1;
        }

        curr_pos += offset;
        curr_cmd &= MASK_CMD;
        ir_code[num_cmd++].command = curr_cmd;
    }

    intrm_repres->size = num_cmd;
}

//-----------------------------------------------------------------------------

void IrDump (Intrm_represent *intrm_repres)
{
    printf ("-- generate dump Intermediate Representation\n\n");

    FILE *dump_file = fopen ("binary_translator/dump/ir_dump.txt", "w+");

    fprintf (dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                       Intermediate Representation Dump                      \n"
            "-----------------------------------------------------------------------------\n\n");

    fprintf (dump_file, "- Intermediate Representation size: %d\n\n", intrm_repres->size);

    //-----------------------------------------------------------------------------  
    // to find out commands numeration check processor/COMMON/include/codegen/codegen.h
    //-----------------------------------------------------------------------------

    for(int i = 0; i < intrm_repres->size; i++)
    {
        fprintf (dump_file,
                "- IR code %d\n"
                "       - command:   %d\n"
                "       - imm_value: %d\n"
                "       - reg_num:   %d\n"
                "       - ram_flag:  %d\n",
                i,
                ir_code[i].command,
                ir_code[i].imm_value,
                ir_code[i].reg_num,
                ir_code[i].ram_flag        );
    }

    printf ("-- successful dumping\n\n");

    fclose (dump_file);
}

#undef ir_code

//-----------------------------------------------------------------------------