#include "backend/include/lines_handle.h"

//-----------------------------------------------------------------------------

int comp_strs_by_begin (void *first_p, void *second_p)
{
    Line *first_line  = (Line*) first_p;
    Line *second_line = (Line*) second_p;

    char *begin_line_1 = first_line->begin_line;
    char *begin_line_2 = second_line->begin_line;

    while(!isalpha(*begin_line_1))
    {
        begin_line_1++;
    }

    while(!isalpha(*begin_line_2))
    {
        begin_line_2++;
    }

    return strcmp (begin_line_1, begin_line_2);
}

//-----------------------------------------------------------------------------

int comp_strs_by_end (void *first_p, void *second_p)
{
    Line *first_line  = (Line*) first_p;
    Line *second_line = (Line*) second_p;

    char *end_line_1 = first_line->begin_line  + first_line->line_lenght  - 1;
    char *end_line_2 = second_line->begin_line + second_line->line_lenght - 1;

    while(!isalpha(*end_line_1))
    {
        end_line_1--;
    }

    while(!isalpha(*end_line_2))
    {
        end_line_2--;
    }

    int min_len = min (first_line->line_lenght + 1, second_line->line_lenght + 1);

    for(int counter = 0; counter < min_len; counter++)
    {
        if(*end_line_1 != *end_line_2)
        {
            return (*end_line_1 - *end_line_2);
        }

        end_line_1--;
        end_line_2--;
    }

    return SAME_LINES;
}

//-----------------------------------------------------------------------------

int min (int first_i, int second_i)
{
    if(first_i > second_i)
    {
        return first_i;
    }

    return second_i;
}

//-----------------------------------------------------------------------------

bool is_equal (double a, double b)
{
    const double EPS = 1e-3;

    return (a - b < EPS && a - b > -EPS);
}

//-----------------------------------------------------------------------------

Line *lines_separator (File *Oper_file)
{
    Line *Array_struct = (Line*) calloc (Oper_file->num_of_lines, sizeof (Line));

    int num_line = 0;
    int cur_len  = 0;

    int file_len = Oper_file->text_size;

    for(int i = 0; i < file_len; i++)
    {
        if(Oper_file->file_buffer[i] == '\n')
        {
            Oper_file->file_buffer[i] = '\0';

            if(Oper_file->file_buffer[i - 1] == '\r')
            {
                Oper_file->file_buffer[i - 1] = '\0';
            }

            Array_struct[num_line].begin_line  = Oper_file->file_buffer + i - cur_len;
            Array_struct[num_line].line_lenght = cur_len;

            cur_len = 0;
            num_line++;
        }

        else
        {
            cur_len++;
        }
    }

    Array_struct[num_line].begin_line = Oper_file->file_buffer +
                                        Oper_file->text_size -
                                        cur_len;

    num_line++;

    return Array_struct;
}

//-----------------------------------------------------------------------------

void bubble_sort (void* ptr_array,  int num_of_lines,
                  int   comp_strs (void* first_str, void* second_str))
{
    size_t size = sizeof (Line);
    char *pointer = (char*) ptr_array;

    for(int i = 0; i < num_of_lines; i++)
    {
        for(int j = i + 1; j < num_of_lines; j++)
        {
            if(comp_strs (pointer + i * size, pointer + j * size) > 0)
            {
                swap_lines (pointer + i * size, pointer + j * size, size);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void swap_lines (void* first_pointer, void* second_pointer, size_t size_of_struct)
{
    char tmp = 0;

    char *c_first  = (char*) first_pointer;
    char *c_second = (char*) second_pointer;

    for(int i = 0; i < int(size_of_struct); i++)
    {
       tmp = c_first[i];

       c_first[i]  = c_second[i];
       c_second[i] = tmp;
    }
}

//-----------------------------------------------------------------------------

bool is_num (char sym)
{
    return (sym >= '0' && sym <= '9');
}

//-----------------------------------------------------------------------------

void clear_mem (Line *Text, File *File_input)
{
    free (File_input->file_buffer);

    free (File_input);

    free (Text);
}

//-----------------------------------------------------------------------------

