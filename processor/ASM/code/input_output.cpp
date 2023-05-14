#include "ASM/include/input_output.h"

//-----------------------------------------------------------------------------

void handle_args (int argc,          char            *argv[], const Option cmd[],
                  int options_range, Arg_handler_res *Arg_par                    )
{
    for(int arg_num = 1; arg_num < argc; arg_num++)
    {
        for(int i = 0; i < options_range; i++)
        {
            if(strcmp (argv[arg_num], cmd[i].opt_name) == 0)
            {
                arg_num += cmd[i].func (&arg_num, argv, Arg_par);
            }
        }
    }
}

//-----------------------------------------------------------------------------

int set_input_file (CMD_FUNC_ARGS)
{
    Arg_par->file_in = fopen ((char*) argv[*arg_num + 1], "rb");

    return SKIP_ARG;
}

//-----------------------------------------------------------------------------

int set_output_file (CMD_FUNC_ARGS)
{
    Arg_par->file_out = fopen ((char*) argv[*arg_num + 1], "w+");

    return SKIP_ARG;
}

//-----------------------------------------------------------------------------

int comp_par_begin (CMD_FUNC_ARGS)
{
    Arg_par->comp_par = comp_strs_by_begin;

    return 0;
}

//-----------------------------------------------------------------------------

int comp_par_end (CMD_FUNC_ARGS)
{
    Arg_par->comp_par = comp_strs_by_end;

    return 0;
}

//-----------------------------------------------------------------------------

int get_num_of_strs (File *File_input)
{
    int string_counter = 0;

    for(int pos = 0; pos < File_input->text_size; pos++)
    {
        if(File_input->file_buffer[pos] == '\n')
        {
            string_counter++;
        }
    }

    string_counter++;

    return string_counter;
}

//-----------------------------------------------------------------------------

int get_file_size (FILE *file)
{
    struct stat file_stat = {0};

    fstat (fileno (file), &file_stat);

    return file_stat.st_size;
}

//-----------------------------------------------------------------------------

File *file_reader (FILE *file)
{
    File *Readed_file = (File*) calloc (1, sizeof (File));

    Readed_file->text_size   = get_file_size (file) + 1;
    Readed_file->file_buffer = (char*) calloc (Readed_file->text_size, sizeof (char));

    fread (Readed_file->file_buffer, sizeof (char), Readed_file->text_size, file);

    Readed_file->file_buffer[Readed_file->text_size - 1] = '\0';
    Readed_file->num_of_lines = get_num_of_strs (Readed_file);

    return (Readed_file);
}

//-----------------------------------------------------------------------------

int file_printer (Line *Text, int num_of_lines, FILE *file)
{
    for(int pos = 0; pos < num_of_lines; pos++)
    {
        fprintf (file, "\n%s", Text[pos].begin_line);
    }

    return 0;
}

//-----------------------------------------------------------------------------
