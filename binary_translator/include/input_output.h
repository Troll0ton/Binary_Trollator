//! @file input_output.h

#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

FILE *openFile (char *name, char *mode, FILE *log_file);

int getFileSize (FILE *file);

//-----------------------------------------------------------------------------

#endif //INPUT_OUTPUT_H
