#include "binary_translator/include/IR-parser.h" 

//-----------------------------------------------------------------------------

Guest_code *readCodeFile (FILE *code_file, FILE *log_file)
{
    log_print ("- traslate to IR                     \n\n"
               "__________|Read Guest code|__________\n\n"
               "- open the code.bin                  \n\n");

    Guest_code *guest_code = (Guest_code*) calloc (1, sizeof (Guest_code));
    checkAlloc (guest_code);

    guest_code->size = getFileSize (code_file);

    guest_code->buffer = (char*) calloc (guest_code->size, sizeof (char));
    checkAlloc (guest_code->buffer);

    fread (guest_code->buffer, sizeof (char), guest_code->size, code_file);

    int code_signature = *(int*)(guest_code->buffer);

    if(code_signature != GUEST_CODE_SIGNATURE)
    {
        err_print ("ERROR: wrong signature!\n\n");
        free (guest_code);

        return NULL;
    }

    log_print ("    - guest code size: %d   \n\n"
               "    - code.bin SIGNATURE: %X\n\n", 
                guest_code->size,
                (unsigned int) code_signature     );

    log_print ("- successful copying\n\n");

    return guest_code;
}

//-----------------------------------------------------------------------------

#define CURR_IR_NODE ir->buffer[num_cmd]

IR *translateGuestToIr (Guest_code *guest_code, FILE *log_file)
{
    log_print ("__________|Translating to IR|__________\n\n"
               "- handle guest code                    \n\n");

    IR *ir = (IR*) calloc (1, sizeof (IR));
    checkAlloc (ir);

    ir->buffer = (IR_node*) calloc (guest_code->size, sizeof (IR_node));
    checkAlloc (ir->buffer);

    log_print ("    - ir start size: %d\n\n", guest_code->size);

    handleGuestCode (ir, guest_code, log_file);

    log_print ("    - ir final size: %d       \n\n"  
               "- successful translating to IR\n\n",
                ir->size + 1                        );

    return ir;
}

//-----------------------------------------------------------------------------

void handleGuestCode (IR *ir, Guest_code *guest_code, FILE *log_file)
{
    int num_cmd = 0;
                                                                               // skip signature
    for(int curr_pos = SIZE_OF_CODE_SIGNATURE; curr_pos < guest_code->size; curr_pos++)
    {       
        CURR_IR_NODE.address.guest = curr_pos;                                 // this is position in guest code

        curr_pos += handleBinMask (&ir->buffer[num_cmd], guest_code, curr_pos);            

        num_cmd++;
    }

    ir->size = num_cmd;

    translateGuestJmpTargets (ir, log_file);
}

//-----------------------------------------------------------------------------

// translateInstrToIr
int handleBinMask (IR_node *ir_node, Guest_code *guest_code, int curr_pos)
{
    int curr_cmd = guest_code->buffer[curr_pos];
    int offset   = 0;

    if(curr_cmd & MASK_REG)
    {
        ir_node->reg_num = (uint8_t) *(double*)(guest_code->buffer + 
                                                curr_pos +                     // rax = 1, ...
                                                SIZE_OF_INSTRUCTION) + 1; 
        offset += SIZE_OF_ARGUMENTS_SPECIFIER;
    }

    if(curr_cmd & MASK_IMM)
    {
        ir_node->imm_val.num = *(double*)(guest_code->buffer + 
                                          curr_pos + 
                                          offset + 
                                          SIZE_OF_INSTRUCTION);
        offset += SIZE_OF_ARGUMENTS_SPECIFIER;
    }

    ir_node->memory_flag = 0;
            
    if(curr_cmd & MASK_MEM)
    {
        ir_node->memory_flag = 1;
    }

    curr_cmd &= MASK_CMD;
    ir_node->command = curr_cmd;
    
    return offset;
}

//-----------------------------------------------------------------------------

void translateGuestJmpTargets (IR *ir, FILE *log_file)                         // changed addressing of jumps into Intermediate Representation
{
    for(int i = 0; i < ir->size; i++)
    {
        switch(ir->buffer[i].command)
        {
            COMMON_JMP_CASE
            {
                searchForTarget (ir, &ir->buffer[i], log_file);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void searchForTarget (IR *ir, IR_node *ir_node, FILE *log_file)
{
    ir_node->imm_val.target = (int) ir_node->imm_val.num;

    for(int num_cmd = 0; num_cmd < ir->size; num_cmd++)
    {
        if(CURR_IR_NODE.address.guest == ir_node->imm_val.target)              // translate target absolute address
        {
            ir_node->imm_val.target = num_cmd;
            return;
        }
    } 

    err_print ("ERROR: jump targets translating!\n\n");
}

//-----------------------------------------------------------------------------

void irDump (IR *ir, FILE *log_file)
{
    log_print ("- generate dump IR\n\n");

    FILE *dump_file = fopen ("binary_translator/dump/ir_dump.txt", "w+");
    checkFilePtr (dump_file);

    fprintf (dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                       Intermediate Representation Dump                      \n"
            "-----------------------------------------------------------------------------\n\n"
            "- intermediate representation size: %d\n\n", 
            ir->size                                                                           );

    for(int num_cmd = 0; num_cmd < ir->size; num_cmd++)
    {
        fprintf (dump_file, "- IR node %d\n", num_cmd);

        #define CMD_DEF(cmd, name, ...)                            \
        case cmd:                                                  \
        {                                                          \
            fprintf (dump_file, "       - command:   %s\n", name); \
            break;                                                 \
        }

        switch(CURR_IR_NODE.command)
        {
            //-----------------------------------------------------------------------------

            #include "processor/COMMON/include/codegen/codegen.h"

            //-----------------------------------------------------------------------------

            default:
                fprintf (dump_file, "       - NULL \n\n");
                break;
        }

        fprintf (dump_file, "       - imm_value: %0.1lf \n"
                            "       - ram_flag:  %u     \n",
                            CURR_IR_NODE.imm_val.num,
                            CURR_IR_NODE.memory_flag        );

        if(CURR_IR_NODE.reg_num)
        {
            fprintf (dump_file, "       - reg:       R%cX\n",
                                CURR_IR_NODE.reg_num + 'A' - 1);
        }
    }

    log_print ("- successful dumping\n\n");

    fclose (dump_file);
}

#undef CURR_IR_NODE

//-----------------------------------------------------------------------------

void irDtor (IR *ir)
{
    free (ir->buffer);
    ir->size = DELETED;
}

//-----------------------------------------------------------------------------

void guestCodeDtor (Guest_code *guest_code)
{
    free (guest_code->buffer);
    guest_code->size = DELETED;
}

//-----------------------------------------------------------------------------