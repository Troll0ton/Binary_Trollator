#include "binary_translator/include/IR-parser.h" 

//-----------------------------------------------------------------------------

Guest_code *readGuestFile (FILE *code_file, FILE *log_file)
{
    logPrint ("- reading guest code file\n\n");

    Guest_code *guest_code = (Guest_code*) calloc (1, sizeof (Guest_code));

    if(!guest_code)
    {
        showError ("guest_code allocation");
    }

    guest_code->size = getFileSize (code_file);

    guest_code->buffer = (char*) calloc (guest_code->size, sizeof (char));

    if(!guest_code->buffer)
    {
        showError ("guest_code->buffer allocation");
    }

    fread (guest_code->buffer, sizeof (char), guest_code->size, code_file);

    int code_signature = *(int*)(guest_code->buffer);

    if(code_signature != GUEST_CODE_SIGNATURE)
    {
        errPrint ("ERROR: wrong signature!\n\n");
        free (guest_code);

        return NULL;
    }

    logPrint (" - guest code size:    %d\n\n"
              " - code.bin signature: %X\n\n", 
              guest_code->size,
              (unsigned int) code_signature   );

    logPrint ("- successful reading guest code file\n\n");

    return guest_code;
}

//-----------------------------------------------------------------------------

#define CURR_IR_NODE ir->buffer[ir->size]

void parseGuestCode (Guest_code *guest_code, IR *ir, FILE *log_file)
{
    logPrint ("- parse guest code\n\n");

    // skip signature
    for(int curr_pos = SIZE_OF_CODE_SIGNATURE; curr_pos < guest_code->size; curr_pos++)
    {       
        irResize (ir, log_file);

        CURR_IR_NODE.address.guest = curr_pos;                                 // this is position in guest code

        curr_pos += translateInstrToIr (&CURR_IR_NODE, guest_code, curr_pos);            

        ir->size++;
    }

    translateGuestJmpTargets (ir, log_file);                                    // after handling all code, fill skipped addresses
                                                                                // (in some instructions)

    logPrint ("- successful parsing guest code\n\n");
};

#undef CURR_IR_NODE

//-----------------------------------------------------------------------------

int translateInstrToIr (IR_node *ir_node, Guest_code *guest_code, int curr_pos)
{
    int curr_cmd = guest_code->buffer[curr_pos];
    int offset   = 0;

    if(curr_cmd & MASK_REG)
    {
        ir_node->reg_num = (uint8_t) *(double*)(guest_code->buffer + 
                                                curr_pos +                     // rax = 1, ...
                                                SIZE_OF_OPCODE) + 1; 
        offset += SIZE_OF_ARGUMENT_SPECIFIER;
    }

    if(curr_cmd & MASK_IMM)
    {
        ir_node->imm_val.num = *(double*)(guest_code->buffer + 
                                          curr_pos + 
                                          offset + 
                                          SIZE_OF_OPCODE);
        offset += SIZE_OF_ARGUMENT_SPECIFIER;
    }

    ir_node->is_mem_access = 0;
            
    if(curr_cmd & MASK_MEM)
    {
        ir_node->is_mem_access = 1;
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

#define CURR_IR_NODE ir->buffer[num_cmd]

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

    errPrint ("ERROR: jump targets translating!\n\n");
}

//-----------------------------------------------------------------------------

void irDump (IR *ir, FILE *log_file)
{
    logPrint ("- generate dump IR\n\n");

    FILE *dump_file = fopen ("binary_translator/dump/ir_dump.txt", "w+");

    if(!dump_file)
    {
        showError ("dump_file opening");
    }

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
                            CURR_IR_NODE.is_mem_access      );

        if(CURR_IR_NODE.reg_num)
        {
            fprintf (dump_file, "       - reg:       R%cX\n",
                                CURR_IR_NODE.reg_num + 'A' - 1);
        }
    }

    logPrint ("- successful dumping\n\n");

    fclose (dump_file);
}

#undef CURR_IR_NODE

//-----------------------------------------------------------------------------

IR *irCtor (FILE *log_file)
{
    IR *ir = (IR*) calloc (1, sizeof (IR));

    if(!ir)
    {
        showError ("ir allocation");
    }

    ir->buffer = (IR_node*) calloc (IR_INIT_SIZE, sizeof (IR_node));
    
    if(!ir->buffer)
    {
        showError ("ir->buffer allocation");
    }

    ir->capacity = IR_INIT_SIZE;

    return ir;
}

//-----------------------------------------------------------------------------

void irResize (IR *ir, FILE *log_file)
{
    if(ir->capacity <= ir->size)
    {
        #ifndef NDEBUG
        logPrint ("IR was resized | prev capacity: %d, curr capacity: %d\n\n",
                   ir->capacity, ir->capacity + IR_INCREASE_PAR               );
        #endif

        ir->capacity += IR_INCREASE_PAR;

        ir->buffer = (IR_node*) realloc (ir->buffer, ir->capacity * sizeof (IR_node));
    }
} 

//-----------------------------------------------------------------------------

void irDtor (IR *ir, FILE *log_file)
{
    free (ir->buffer);

    #ifndef NDEBUG
    logPrint ("IR was deleted | prev size: %d, prev capacity: %d\n\n",
               ir->size, ir->capacity                                 );
    #endif

    ir->capacity = DELETED;
    ir->size     = DELETED;
}

//-----------------------------------------------------------------------------

void guestCodeDtor (Guest_code *guest_code)
{
    free (guest_code->buffer);
    guest_code->size = DELETED;
}

//-----------------------------------------------------------------------------