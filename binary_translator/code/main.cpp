#include "binary_translator/include/IR-parser.h"

//-----------------------------------------------------------------------------

int main ()
{
    printf ("-- traslate to IR\n\n");

    FILE *code_file = fopen ("processor/COMMON/files/code.bin", "rb");

    Bin_code *bin_code = read_code_file (code_file);

    translateBinToIR (code_file);

    fclose (code_file);

    return 0;
}

//-----------------------------------------------------------------------------