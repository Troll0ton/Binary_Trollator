//! @file common.h <- Binary translator

#ifndef BT_COMMON_H
#define BT_COMMON_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <elf.h>
#include <limits.h>
#include <sys/mman.h>

//-----------------------------------------------------------------------------

#define ELF_MODE 1

//-----------------------------------------------------------------------------

#define N(suffix) NUM_OF_##suffix
#define O(suffix) OFFSET_##suffix
#define S(suffix) SIZE_OF_##suffix

//-----------------------------------------------------------------------------

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

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

enum ELF_INFO
{
    NUM_OF_SEGMENTS = 3,
    LOAD_ADDR   = 0x400000,
    TEXT_ADDR   = LOAD_ADDR + sizeof (Elf64_Ehdr) + 
                  NUM_OF_SEGMENTS * sizeof (Elf64_Phdr),  
    RAM_ADDR    = TEXT_ADDR + PAGE_SIZE, 
    FUNCT_ADDR  = RAM_ADDR  + PAGE_SIZE, 
    TOTAL_SIZE  = sizeof (Elf64_Ehdr) + 
                  NUM_OF_SEGMENTS * (sizeof (Elf64_Phdr) + PAGE_SIZE),
};

//-----------------------------------------------------------------------------

typedef double   signature;
typedef double   elem_t;
typedef uint32_t cmd_code;

//-----------------------------------------------------------------------------

#define CONDITIONAL_JMP_CASE  \
    case JBE:                 \
    case JAE:                 \
    case JA:                  \
    case JB:                  \
    case JE:                  \
    case JNE:  

//-----------------------------------------------------------------------------  

#define COMMON_JMP_CASE       \
    CONDITIONAL_JMP_CASE      \
    case JMP:                 \
    case CALL:                 

//-----------------------------------------------------------------------------

enum CMD_CODES
{
    #define CMD_DEF(cmd, ...) \
        cmd,

    #include "processor/COMMON/include/codegen/codegen.h"

    #undef CMD_DEF
};

//-----------------------------------------------------------------------------

enum TRANSLATOR_INFO
{
    ABSENCE        = 0xBEDBAD,
    DELETED        = 0xDEDAC,
    MPROTECT_ERROR = -1,
};

//-----------------------------------------------------------------------------

#endif // BT_COMMON_H