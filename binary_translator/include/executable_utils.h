//! @file executable_utils.h

#ifndef EXECUTABLE_H
#define EXECUTABLE_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"
#include "binary_translator/include/translator.h" 

//-----------------------------------------------------------------------------
//                                  ELF
//-----------------------------------------------------------------------------
// ELF header
// Program headers:
//      text        - size: PAGESIZE
//      ram         - size: PAGESIZE
//      lib (print) - size: PAGESIZE
// Code
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void programHeaderInit (char **curr_pos, int pos, Elf64_Word p_flags);

void runCode (char *code, int size);

void createELF (X64_code *x64_code);

void writeInBinCode (char *pos, char *file_name, X64_code *x64_code);

//-----------------------------------------------------------------------------

#endif // EXECUTABLE_H