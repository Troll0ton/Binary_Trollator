//! @file input_output.h

#ifndef   INPUT_OUTPUT_H
#define   INPUT_OUTPUT_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys\stat.h>
#include <cctype>

//-----------------------------------------------------------------------------

#include "lines_handle.h"

//-----------------------------------------------------------------------------

#define SKIP_ARG 1

//-----------------------------------------------------------------------------

#define CMD_FUNC_ARGS int  *arg_num,\
                      char *argv[],\
                      struct Arg_handler_res *Arg_par

//-----------------------------------------------------------------------------

const int num_of_support_args = 4;

//-----------------------------------------------------------------------------

typedef struct Option
{
    char *opt_name;

    int (*func)(CMD_FUNC_ARGS);
} Option;

//-----------------------------------------------------------------------------

typedef struct File
{
    char *file_buffer;

    int   text_size;

    int   num_of_lines;
} File;

//-----------------------------------------------------------------------------

typedef struct Line
{
    char *begin_line;

    int   line_lenght;
} Line;

//-----------------------------------------------------------------------------

typedef struct Arg_handler_res
{
    FILE *file_in;
    FILE *file_out;

    int (*comp_par)(void *first_p, void *second_p);
} Arg_handler_res;

//-----------------------------------------------------------------------------

void  handle_args     (int argc,          char            *argv[], const Option cmd[],
                       int options_range, Arg_handler_res *Arg_par                    );

int   set_input_file  (CMD_FUNC_ARGS);

int   set_output_file (CMD_FUNC_ARGS);

int   comp_par_begin  (CMD_FUNC_ARGS);

int   comp_par_end    (CMD_FUNC_ARGS);

int   get_num_of_strs (File *File_input);

File *file_reader     (FILE *file);

int   get_file_size   (FILE *file);

int   file_printer    (Line *Text, int num_of_lines, FILE *file);

//-----------------------------------------------------------------------------

const Option cmd_args[] =
{
    {"--begin",  comp_par_begin },
    {"--end",    comp_par_end   },

    {"-if",      set_input_file },
    {"-of",      set_output_file},
};

//-----------------------------------------------------------------------------

#endif //INPUT_OUTPUT_H
