#include "binary_translator/include/IR-parser.h" 

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
        *(elem_t*)(bin_code->buffer + OFFSET_ARG) = code_signature;

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

#define curr_node ir->buffer 

IR *translateBinToIR (Bin_code *bin_code)
{
    printf ("-- translating to Intermediate Representation\n\n");

    IR *ir = (IR*) calloc (1, sizeof (IR));
    ir->buffer = (Ir_node*) calloc (bin_code->size, sizeof (Ir_node));

    printf ("-------- ir start size: %d\n\n", bin_code->size);

    handleBinCode (ir, bin_code);

    printf ("-------- ir final size: %d\n\n", ir->size + 1);
    printf ("-- successful translating\n\n");

    return ir;
}

//-----------------------------------------------------------------------------

void handleBinCode (IR *ir, Bin_code *bin_code)
{
    int num_cmd = 0;

    for(int curr_pos = 2 * OFFSET_ARG; curr_pos < bin_code->size; curr_pos++)
    {
        int curr_cmd = bin_code->buffer[curr_pos];
        curr_node[num_cmd].bin_pos = curr_pos; // here we input pos in our bin file

        int offset = 0;

        if(curr_cmd & MASK_REG)
        {
            curr_node[num_cmd].reg_value = (int) *(elem_t*)(bin_code->buffer + curr_pos + OFFSET_CMD) + 1; // rax = 1, ...
            offset += OFFSET_ARG;
        }

        if(curr_cmd & MASK_IMM)
        {
            curr_node[num_cmd].imm_value = (int) *(elem_t*)(bin_code->buffer + curr_pos + offset + OFFSET_CMD);
            offset += OFFSET_ARG;
        }

        curr_node[num_cmd].ram_flag = 0;
        
        if(curr_cmd & MASK_RAM)
        {
            curr_node[num_cmd].ram_flag = 1;
        }

        curr_pos += offset;
        curr_cmd &= MASK_CMD;

        curr_node[num_cmd].command = curr_cmd;
        num_cmd++;
    }

    ir->size = num_cmd;
}

//-----------------------------------------------------------------------------

void IrDump (IR *ir)
{
    printf ("-- generate dump Intermediate Representation\n\n");

    FILE *dump_file = fopen ("binary_translator/dump/ir_dump.txt", "w+");

    fprintf (dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                       Intermediate Representation Dump                      \n"
            "-----------------------------------------------------------------------------\n\n");

    fprintf (dump_file, "- Intermediate Representation size: %d\n\n", ir->size);

    //-----------------------------------------------------------------------------  
    // to find out commands numeration check processor/COMMON/include/codegen/codegen.h
    //-----------------------------------------------------------------------------

    for(int i = 0; i < ir->size; i++)
    {
        fprintf (dump_file,
                "- IR node %d\n", i);

        #define CMD_DEF(cmd, name, ...)             \
        case cmd:                                   \
        {                                           \
            fprintf (dump_file,                     \
                "       - command:   %s\n", name);  \
            break;                                  \
        }

        switch(ir->buffer[i].command)
        {
            #include "processor/COMMON/include/codegen/codegen.h"

            default:
                fprintf (dump_file,
                "       - NUL \n"  );
                break;
        }

        fprintf (dump_file,
                "       - reg_value: %d\n"
                "       - imm_value: %d\n"
                "       - ram_flag:  %d\n",
                curr_node[i].reg_value,
                curr_node[i].imm_value,
                curr_node[i].ram_flag        );
    }

    printf ("-- successful dumping\n\n");

    fclose (dump_file);
}

#undef curr_node

//-----------------------------------------------------------------------------

void IntrmRepresentDtor (IR *ir)
{
    free (ir->buffer);
    ir->size = deleted;
}

//-----------------------------------------------------------------------------

void BinCodeDtor (Bin_code *bin_code)
{
    free (bin_code->buffer);
    bin_code->size = deleted;
}

//-----------------------------------------------------------------------------