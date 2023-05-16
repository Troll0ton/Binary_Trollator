//! @file translator.h 

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

//-----------------------------------------------------------------------------

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

void translateIrToX86 (Intrm_represent *intrm_repres);

void runCode (char *buffer);

//-----------------------------------------------------------------------------

#endif //TRANSLATOR_H