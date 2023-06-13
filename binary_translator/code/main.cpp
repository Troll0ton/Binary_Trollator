#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/translator.h"
#include "binary_translator/include/common.h"

//----------------------------------------------------------------------------

int main ()
{
    printf ("-- traslate to IR\n\n");

    FILE *code_file = fopen ("processor/COMMON/files/code.bin", "rb");

    Troll_code *bin_code = readCodeFile (code_file);
    int bin_size = bin_code->size;
    
    IR *ir = translateBinToIr (bin_code);
    IrDump (ir);

    BinCodeDtor (bin_code);
    fclose (code_file);
    
    X64_code *x64_code = translateIrToX64 (ir, bin_size);
    CodeX64Dump (x64_code->buffer, x64_code->size);
    IrDtor (ir);

    runCode (x64_code->buffer, x64_code->size);
    X64RepresentDtor (x64_code);

    printf ("-- finishing\n\n");

    return 0;
}

//-----------------------------------------------------------------------------