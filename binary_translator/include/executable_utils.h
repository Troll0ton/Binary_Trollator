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
//      memory      - size: PAGESIZE
//      lib (print) - size: PAGESIZE
// Code
//-----------------------------------------------------------------------------

enum ELF_INFO
{
    NUM_OF_SEGMENTS = 3,
    LOAD_ADDR       = 0x400000,
    TEXT_ADDR       = 0x4000E8,          
    MEMORY_ADDRESS  = 0x4010E8, 
    FUNCT_ADDR      = 0x4020E8, 
    TOTAL_SIZE      = 0x30E8,
};

//-----------------------------------------------------------------------------

void programHeaderInit (char **curr_pos, int pos, Elf64_Word p_flags);

void runCode (char *code, int size);

void createELF (X64_code *x64_code);

void writeInBinCode (char *pos, char *file_name, X64_code *x64_code);

//-----------------------------------------------------------------------------

#endif // EXECUTABLE_H