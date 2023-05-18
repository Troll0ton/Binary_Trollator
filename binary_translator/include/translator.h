//! @file translator.h 

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

const uint32_t tmp_size = 20;

//-----------------------------------------------------------------------------

const uint32_t size_ptr = 4;
const uint32_t size_abs_ptr = 8;
const uint32_t size_num = 8;

//-----------------------------------------------------------------------------

X86_represent *translateIrToX86 (Intrm_represent *intrm_repres, int bin_size);

void runCode (char *code, int size);

void emitCmd (char **code, char *cmd, int size);

void emitPtr (char **code, unsigned int addr);

void emitAbsPtr (char **code, unsigned long long int addr);

void emitNum (char **code, double num);

void handleMprotextError (int mprotect_status);

void Codex86Dump (char *code, int size);

int double_printf (double *value);

void X86RepresentDtor (X86_represent *x86_represent);

//-----------------------------------------------------------------------------

#endif //TRANSLATOR_H