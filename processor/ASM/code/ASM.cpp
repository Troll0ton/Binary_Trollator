#include "ASM/include/ASM.h"

//-----------------------------------------------------------------------------

int assembler_ctor (Assembler *Asm, int argc, char *argv[])
{                                   //INITIALIZING
    Asm->code.array  = (char*) calloc (CODE_SIZE,  sizeof (char));
    Asm->label.array = (int*)  calloc (LABEL_SIZE, sizeof (int));

    if(!Asm->code.array)
    {
        printf ("|ERROR - Null pointer, code array|\n");

        return ERROR_CTOR;
    }

    if(!Asm->label.array)
    {
        printf ("|ERROR - Null pointer, label array|\n");

        return ERROR_CTOR;
    }
                        //OFFSET
    Asm->code.size      = SIZE_OF_CODE_SIGNATURE;
    Asm->code.capacity  = CODE_SIZE;

    Asm->label.size     = OFFSET_LABEL_SIGNATURE;
    Asm->label.capacity = LABEL_SIZE;

    Asm->curr_pos = SIZE_OF_CODE_SIGNATURE;
    Asm->offset   = 0;

    Asm->info      = { 0 };
    Asm->curr_line = { 0 };

    return asm_info_ctor (&Asm->info, argc, argv);
}

//-----------------------------------------------------------------------------

#define DOUBLE_PASS (info->double_pass)

int asm_info_ctor (Asm_info *info, int argc, char *argv[])
{
    info->code_signature = SIGNATURE;

    if(argc == 1)
    {
        info->file_in = fopen ("COMMON/files/input.asm", "rb");
    }

    else if(argc == 2)
    {
        info->file_in = fopen (argv[1], "rb");
    }

    else
    {
        printf ("UNDEFINED ARGUMENTS OF COMMAND LINE!\n\n");
    }

    info->code_file = fopen ("processor/COMMON/files/code.bin", "wb");

    DOUBLE_PASS = false;

    if(!info->file_in)
    {
        printf ("|ERROR - file_in opening|\n");

        return ERROR_CTOR;
    }

    if(!info->code_file)
    {
        printf ("|ERROR - code_file opening|\n");

        return ERROR_CTOR;
    }

    return 0;
}

#undef DOUBLE_PASS

//-----------------------------------------------------------------------------

void assembling (Assembler *Asm)
{
    File *File_input = file_reader     (Asm->info.file_in);
    Line *Text       = lines_separator (File_input);

    parse_text     (Asm, Text, File_input);
    write_res_sums (Asm);
    clear_mem      (Text, File_input);

    fwrite (Asm->code.array, sizeof (char), Asm->code.size, Asm->info.code_file);
}

//-----------------------------------------------------------------------------

#define DOUBLE_PASS (Asm->info.double_pass)

void parse_text (Assembler *Asm, Line *Text, File *File_input)
{
    DOUBLE_PASS = false;

    for(int i = 0; i < File_input->num_of_lines; i++)
    {
        Command cmd  = { 0 };
        Argument arg = { 0 };

        Asm->curr_line = Text[i];

        parse_line    (Asm, &cmd, &arg);
        write_in_code (Asm,  cmd,  arg);
    }

    Asm->code.size = Asm->curr_pos;
    Asm->curr_pos  = SIZE_OF_CODE_SIGNATURE;

    if(DOUBLE_PASS)
    {
        // do it again (with already filled jmp table)
        parse_text (Asm, Text, File_input);
    }
}

#undef DOUBLE_PASS

//-----------------------------------------------------------------------------

void parse_line (Assembler *Asm, Command *cmd, Argument *arg)
{
    parse_label (Asm, arg);
    parse_cmd   (Asm, cmd, arg);
    parse_arg   (Asm, cmd, arg);
}

//-----------------------------------------------------------------------------

void parse_label (Assembler *Asm, Argument *arg)
{
    if(lscan (":%lg", &arg->value) == 1)
    {
        if(arg->value > Asm->label.size)
        {
            Asm->label.size = (int) arg->value;
        }

        arg->is_label = true;
    }
}

//-----------------------------------------------------------------------------

void parse_cmd (Assembler *Asm, Command *cmd, Argument *arg)
{
    char cmd_name[MAX_LEN] = "";

    if(!arg->is_label && lscan("%20s", cmd_name) == 1)
    {
        char *cmd_names[] =
        {
            #define CMD_DEF(cmd, name, code, ...) \
                name,

            #include "COMMON/include/codegen/codegen.h"

            #undef CMD_DEF
        };

        for(int num_cmd = 0; num_cmd < NUM_OF_SUPPORTED_CMD; num_cmd++)
        {
            if(strcmp (cmd_name, cmd_names[num_cmd]) == 0)
            {
                cmd->code     = num_cmd;
                cmd->is_label = true;

                break;
            }
        }

        if(!cmd->is_label) 
        {
            Asm->info.code_signature = SIGNATURE_DESTROYED;
        }
    }
}

//-----------------------------------------------------------------------------

#define DOUBLE_PASS (Asm->info.double_pass)

void parse_arg (Assembler *Asm, Command *cmd, Argument *arg)
{
    if(cmd->is_label)
    {
        char unexpected_line[MAX_LEN] = "";

        if(strchr (Asm->curr_line.begin_line, ':') != NULL)
        {
            if(lscan ("%*s %lg", &arg->value) == 1)
            {
                cmd->mask |= MASK_IMM;

                search_label (Asm, arg);
            }

            else
            {
                Asm->info.code_signature = SIGNATURE_DESTROYED;
            }
        }

        #define PARSE_ARG(num, name_msk, format, ...)     \
            else if(lscan (format, __VA_ARGS__) == num)   \
            {                                             \
                cmd->mask |= name_msk;                    \
            }

        //-----------------------------------------------------------------------------

        #include "COMMON/include/codegen/asm_codegen.h"

        //-----------------------------------------------------------------------------

        #undef PARSE_ARG

        else if(lscan ("%*s %20s", unexpected_line) == 1)
        {
            printf ("ERROR - UNEXPECTED LINE!\n");

            Asm->info.code_signature = SIGNATURE_DESTROYED;
        }
    }
}

#undef DOUBLE_PASS

//-----------------------------------------------------------------------------

#define DOUBLE_PASS (Asm->info.double_pass)

void search_label (Assembler *Asm, Argument *arg)
{
    if(arg->value > Asm->label.size || Asm->label.array[(int) arg->value] <= 0)
    {
        arg->value = NOT_FOUND;

        DOUBLE_PASS = true;
    }

    else
    {
        arg->value = Asm->label.array[(int) arg->value];
    }
}

#undef DOUBLE_PASS

//-----------------------------------------------------------------------------

void write_in_code (Assembler *Asm, Command cmd, Argument arg)
{
    if(Asm->curr_pos + SIZE_DIFFERENCE > Asm->code.capacity)
    {
        Asm->code.capacity *= PAR_INCREASE;

        Asm->code.array = (char*) recalloc (Asm->code.array,
                                            Asm->code.capacity,
                                            Asm->curr_pos,
                                            sizeof (char)      );
    }

    if(!cmd.is_label && arg.is_label)
    {
        write_in_label (Asm, arg);
    }

    else if(cmd.is_label)
    {
        Asm->code.array[Asm->curr_pos] = (char) (cmd.code |= cmd.mask);
        Asm->curr_pos += OFFSET_CMD;

        write_in_arg (Asm, cmd, arg);
    }
}

//-----------------------------------------------------------------------------

void write_in_label (Assembler *Asm, Argument arg)
{
    if(Asm->label.size + SIZE_DIFFERENCE > Asm->label.capacity)
    {
        Asm->label.capacity *= 2;

        Asm->label.array = (int*) recalloc (Asm->label.array,
                                            Asm->label.capacity,
                                            Asm->label.size,
                                            sizeof (int)        );
    }

    if(arg.value > Asm->label.size)
    {
        Asm->label.size = (int) arg.value;
    }

    Asm->label.array[(int) arg.value] = Asm->curr_pos;
}

//-----------------------------------------------------------------------------

void write_in_arg (Assembler *Asm, Command cmd, Argument arg)
{
    if(cmd.code & MASK_REG)
    {
        *(elem_t*)(Asm->code.array + Asm->curr_pos) = (elem_t) (arg.reg_sym - 'a');

        Asm->curr_pos += OFFSET_ARG;
    }

    if(cmd.code & MASK_IMM)
    {
        *(elem_t*)(Asm->code.array + Asm->curr_pos) = (elem_t) arg.value;

        Asm->curr_pos += OFFSET_ARG;
    }
}

//-----------------------------------------------------------------------------

void assembler_dtor (Assembler *Asm)
{
    free (Asm->code.array);
    free (Asm->label.array);

    Asm->code.size      = DELETED_PAR;
    Asm->code.capacity  = DELETED_PAR;
    Asm->label.size     = DELETED_PAR;
    Asm->label.capacity = DELETED_PAR;

    asm_info_dtor (&Asm->info);
}

//-----------------------------------------------------------------------------

void asm_info_dtor (Asm_info *info)
{
    info->code_signature = SIGNATURE_DESTROYED;

    fclose (info->file_in);
    fclose (info->code_file);
}

//-----------------------------------------------------------------------------

void write_res_sums (Assembler *Asm)
{
    *(int*)(Asm->code.array) = Asm->info.code_signature;

    Asm->label.array[0] = Asm->label.size;
}

//-----------------------------------------------------------------------------

void asm_dump (Assembler *Asm)
{
    FILE *code_dmp_file = fopen ("processor/ASM/dump/code_asm_dump.txt", "w+");

    int num_of_cmd = 1;

    fprintf (code_dmp_file, "%d - size, %X - signature\n",
                            Asm->code.size,
                            (unsigned int) *(int*)(Asm->code.array));

    for(int curr_pos = SIZE_OF_CODE_SIGNATURE; curr_pos < Asm->code.size; curr_pos++)
    {
        char curr_cmd = *(Asm->code.array + curr_pos);
        int  offset   = 0;

        fprintf (code_dmp_file, "%06d - logic pos, %06d - phys pos || %d\n",
                                num_of_cmd, 
                                curr_pos, 
                                (int) Asm->code.array[curr_pos]             );

        num_of_cmd++;

        if(curr_cmd & MASK_REG)
        {
            fprintf (code_dmp_file, "%06d - logic pos, %06d - phys pos || %lg\n",
                                    num_of_cmd, 
                                    curr_pos + OFFSET_CMD,
                                    *(elem_t*)(Asm->code.array + curr_pos + OFFSET_CMD));

            num_of_cmd++;

            offset += OFFSET_ARG;
        }

        if(curr_cmd & MASK_IMM)
        {
            fprintf (code_dmp_file, "%06d - logic pos, %06d - phys pos || %lg\n",
                                    num_of_cmd, 
                                    curr_pos + OFFSET_CMD + offset,
                                    *(elem_t*)(Asm->code.array + curr_pos + OFFSET_CMD + offset));

            num_of_cmd++;

            offset += OFFSET_ARG;
        }

        curr_pos += offset;
    }

    fclose (code_dmp_file);
}

//-----------------------------------------------------------------------------


