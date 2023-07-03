#include "binary_translator/include/input_output.h"

//-----------------------------------------------------------------------------

FILE *openFile (char *name, char *mode, FILE *log_file)
{
    FILE *new_file = fopen (name, mode);
    checkFilePtr (log_file);

    return new_file;
}

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
