//! @file lines_handle.h

#ifndef   LINES_HANDLE_H
#define   LINES_HANDLE_H

//-----------------------------------------------------------------------------

#include "input_output.h"

//-----------------------------------------------------------------------------

#define SAME_LINES 0

//-----------------------------------------------------------------------------

int   comp_strs_by_begin        (void *first_p, void *second_p);

int   min                       (int first_i, int second_i);

int   comp_strs_by_end          (void *first_p, void *second_p);

struct Line *lines_separator    (struct File *Oper_file);

void  swap_lines                (void* first_pointer, void* second_pointer, size_t size_of_struct);

void  bubble_sort               (void* ptr_array,  int num_of_lines,
                                 int   comp_strs (void* first_str, void* second_str));

void  clear_mem                 (struct Line *Text, struct File *File_input);

//-----------------------------------------------------------------------------

#endif //LINES_HANDLE_H
