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

//#define ELF_MODE 1

//-----------------------------------------------------------------------------

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

//-----------------------------------------------------------------------------

#define log_print(...) fprintf (log_file, __VA_ARGS__)

#define err_print(...) fprintf (stderr,   __VA_ARGS__); \
                       fprintf (log_file, __VA_ARGS__); 

//-----------------------------------------------------------------------------

#define checkAlloc(name)                                                    \
    if(!name)                                                               \
    {                                                                       \
        err_print ("ERROR: %s allocation, in file: %s, line: %d\n\n",       \
                    #name,                                                  \
                    __FILE__,                                               \
                    __LINE__                                         );     \
    }

//-----------------------------------------------------------------------------

#define checkFilePtr(name)                                                  \
    if(!name)                                                               \
    {                                                                       \
        err_print ("ERROR: can't open %s, in file: %s, line: %d\n\n",       \
                    #name,                                                  \
                    __FILE__,                                               \
                    __LINE__                                         );     \
    }

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

    //-----------------------------------------------------------------------------

    #include "processor/COMMON/include/codegen/codegen.h"

    //-----------------------------------------------------------------------------

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

enum ERROR_CODES
{
    ERROR_MISS_JMP_TARGET = 1,
};

//-----------------------------------------------------------------------------

#endif // BT_COMMON_H