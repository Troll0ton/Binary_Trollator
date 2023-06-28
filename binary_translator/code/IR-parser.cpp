#include "binary_translator/include/IR-parser.h" 

//-----------------------------------------------------------------------------

Guest_code *readCodeFile (FILE *code_file, FILE *log_file)
{
    log_print ("__________|Read Guest code|__________\n\n"
               "- open the code.bin                  \n\n");

    Guest_code *guest_code = (Guest_code*) calloc (1, sizeof (Guest_code));

    guest_code->size   = get_file_size  (code_file);
    guest_code->buffer = (char*) calloc (guest_code->size, sizeof (char));

    fread (guest_code->buffer, sizeof (char), guest_code->size, code_file);

    // SIGNATURE
    int code_signature = *(int*)(guest_code->buffer);

    if(code_signature != GUEST_CODE_SIGNATURE)
    {
        log_print ("ERROR: wrong signature!\n\n");
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
    log_print ("__________|Translating to IR|__________\n\n");

    IR *ir     = (IR*)      calloc (1,                sizeof (IR));
    ir->buffer = (IR_node*) calloc (guest_code->size, sizeof (IR_node));

    log_print ("-   ir start size: %d\n\n", guest_code->size);

    handleGuestCode (ir, guest_code, log_file);

    log_print ("-   ir final size: %d\n\n"  
               "- successful translating\n\n",
                ir->size + 1                  );

    return ir;
}

//-----------------------------------------------------------------------------

void handleGuestCode (IR *ir, Guest_code *guest_code, FILE *log_file)
{
    int num_cmd = 0;
                        // skip signature and result sum
    for(int curr_pos = OFFSET_CODE_SIGNATURE; curr_pos < guest_code->size; curr_pos++)
    {       
        CURR_IR_NODE.address.guest = curr_pos; // this is position in troll file

        curr_pos += handleBinMask (&ir->buffer[num_cmd], guest_code, curr_pos);            

        num_cmd++;
    }

    ir->size = num_cmd;

    translateGuestJmpTargets (ir, log_file);
}

//-----------------------------------------------------------------------------

int handleBinMask (IR_node *ir_node, Guest_code *guest_code, int curr_pos)
{
    int curr_cmd = guest_code->buffer[curr_pos];
    int offset   = 0;

    if(curr_cmd & MASK_REG)
    {
        ir_node->reg_num = (int) *(double*)(guest_code->buffer + 
                                            curr_pos +        // rax = 1, ...
                                            SIZE_OF_INSTRUCTION + 1) ; 
        offset += SIZE_OF_ARGUMENT_SPECIFIER;
    }

    if(curr_cmd & MASK_IMM)
    {
        ir_node->imm_val.num = *(double*)(guest_code->buffer + 
                                          curr_pos + 
                                          offset + 
                                          SIZE_OF_INSTRUCTION );
        offset += SIZE_OF_ARGUMENT_SPECIFIER;
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

void translateGuestJmpTargets (IR *ir, FILE *log_file)
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

#undef TARGET

//-----------------------------------------------------------------------------

void searchForTarget (IR *ir, IR_node *ir_node, FILE *log_file)
{
    for(int num_cmd = 0; num_cmd < ir->size; num_cmd++)
    {
        // set already known target in jmp
        if(CURR_IR_NODE.troll_pos == *target)
        {
            *target = num_cmd;
            break;
        }
    } 

    log_print ("ERROR: jump targets translating!\n\n");
    exit (ERROR_MISS_JMP_TARGET);
}

//-----------------------------------------------------------------------------

void irDump (IR *ir)
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