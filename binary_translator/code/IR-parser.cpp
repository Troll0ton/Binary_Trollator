#include "binary_translator/include/IR-parser.h" 

//-----------------------------------------------------------------------------

Troll_code *readCodeFile (FILE *code_file)
{
    printf ("-- open the code.bin\n\n");

    Troll_code *bin_code = (Troll_code*) calloc (1, sizeof (Troll_code));

    elem_t code_signature = 0;
    elem_t res_sum        = 0;

    fread (&res_sum,        sizeof(char), OFFSET_ARG, code_file);
    fread (&code_signature, sizeof(char), OFFSET_ARG, code_file);

    bin_code->size = res_sum;

    printf ("-------- bin res sum: %d   \n\n", bin_code->size);
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

    if(code_signature == DESTROYED)
    {
        printf ("This file was destroyed by someone.\n");
    }
    
    free (bin_code);

    return NULL;
}

//-----------------------------------------------------------------------------

#define CURR_IR_NODE ir->buffer[num_cmd]

IR *translateBinToIr (Troll_code *bin_code)
{
    printf ("-- translating to Intermediate Representation\n\n");

    IR *ir = (IR*) calloc (1, sizeof (IR));
    ir->buffer = (IR_node*) calloc (bin_code->size, sizeof (IR_node));

    printf ("-------- ir start size: %d\n\n", bin_code->size);

    handleBinCode (ir, bin_code);

    printf ("-------- ir final size: %d\n\n", ir->size + 1);
    printf ("-- successful translating\n\n");

    return ir;
}

//-----------------------------------------------------------------------------

void handleBinCode (IR *ir, Troll_code *bin_code)
{
    int num_cmd = 0;
                        // skip signature and result sum
    for(int curr_pos = 2 * OFFSET_ARG; curr_pos < bin_code->size; curr_pos++)
    {
        CURR_IR_NODE.troll_pos = curr_pos; // this is position in troll file

        curr_pos += handleTrollMask (&ir->buffer[num_cmd], bin_code, curr_pos);            

        ir->size = ++num_cmd;
    }

    ir->size = num_cmd;

    if(translateJmpTargetsIR (ir) == JUMP_TARGET_EMPTY)
    {
        printf ("ERROR: jump targets translating!\n\n");
    }
}

//-----------------------------------------------------------------------------

int handleTrollMask (IR_node *ir_node, Troll_code *bin_code, int curr_pos)
{
    int curr_cmd = bin_code->buffer[curr_pos];
    int offset = 0;

    if(curr_cmd & MASK_REG)
    {
        ir_node->reg_value = (int) *(elem_t*)(bin_code->buffer + curr_pos + OFFSET_CMD) + IR_REG; // rax = 1, ...
        offset += OFFSET_ARG;
    }

    if(curr_cmd & MASK_IMM)
    {
        ir_node->imm_value = (int) *(elem_t*)(bin_code->buffer + curr_pos + offset + OFFSET_CMD);
        offset += OFFSET_ARG;
    }

    ir_node->ram_flag = 0;
            
    if(curr_cmd & MASK_RAM)
    {
        ir_node->ram_flag = 1;
    }

    curr_cmd &= MASK_CMD;
    ir_node->command = curr_cmd;
    
    return offset;
}

//-----------------------------------------------------------------------------

#define TARGET ir->buffer[i].imm_value

// changed addressing of jumps into Intermediate Representation

int translateJmpTargetsIR (IR *ir)
{
    for(int i = 0; i < ir->size; i++)
    {
        switch(ir->buffer[i].command)
        {
            COMMON_JMP_CASE
            {
                char find_flag = 0;

                for(int num_cmd = 0; num_cmd < ir->size; num_cmd++)
                {
                    if(CURR_IR_NODE.troll_pos == TARGET)
                    {
                        TARGET = num_cmd;
                        find_flag = 1;
                    }

                    if(!CURR_IR_NODE.troll_pos)
                    {
                        break;
                    }
                } 

                if(!find_flag)
                {
                    return JUMP_TARGET_EMPTY;
                }
                
                break;
            }
            default:
                break;
        }
    }

    return 0;
}

#undef TARGET

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

    for(int num_cmd = 0; num_cmd < ir->size; num_cmd++)
    {
        fprintf (dump_file,
                "- IR node %d\n", num_cmd);

        #define CMD_DEF(cmd, name, ...)             \
        case cmd:                                   \
        {                                           \
            fprintf (dump_file,                     \
                "       - command:   %s\n", name);  \
            break;                                  \
        }

        switch(CURR_IR_NODE.command)
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
                CURR_IR_NODE.reg_value,
                CURR_IR_NODE.imm_value,
                CURR_IR_NODE.ram_flag         );
    }

    printf ("-- successful dumping\n\n");

    fclose (dump_file);
}

#undef CURR_IR_NODE

//-----------------------------------------------------------------------------

void IrDtor (IR *ir)
{
    free (ir->buffer);
    ir->size = DELETED;
}

//-----------------------------------------------------------------------------

void BinCodeDtor (Troll_code *bin_code)
{
    free (bin_code->buffer);
    bin_code->size = DELETED;
}

//-----------------------------------------------------------------------------