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

    fclose (code_file);

    X86_code *x86_code = translateIrToX86 (ir, bin_size);
    CodeX86Dump (x86_code->buffer, x86_code->size);

    runCode (x86_code->buffer, x86_code->size);

    BinCodeDtor (bin_code);
    X86RepresentDtor (x86_code);
    IrDtor (ir);

    printf ("-- finishing\n\n");

    return 0;
}

//-----------------------------------------------------------------------------