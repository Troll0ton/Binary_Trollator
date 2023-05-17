//! @file translator.h 

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

X86_represent *translateIrToX86 (Intrm_represent *intrm_repres);

void runCode (char *code, int size);

void emitCmd (char **code, char *cmd, int size);

void emitPtr (char **code, unsigned int addr);

void emitAbsPtr (char **code, unsigned long long int addr);

void emitNum (char **code, long long int num);

void handleMprotextError (int mprotect_status);

void Codex86Dump (char *code, int size);

void printLongLongInt ();

//-----------------------------------------------------------------------------

#endif //TRANSLATOR_H