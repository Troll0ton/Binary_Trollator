#include "binary_translator/include/input_output.h"

//-----------------------------------------------------------------------------

int getFileSize (FILE *file)
{
    int file_size = 0;

    while(fgetc (file) != EOF)
    {
        file_size++;
    }

    fseek (file, 0, SEEK_SET);

    return file_size;
}

//-----------------------------------------------------------------------------
