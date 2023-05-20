#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/translator.h"
#include "binary_translator/include/common.h"

//----------------------------------------------------------------------------

int main ()
{
    printf ("-- traslate to IR\n\n");

    FILE *code_file = fopen ("processor/COMMON/files/code.bin", "rb");

    Bin_code *bin_code = readCodeFile (code_file);
    int bin_size = bin_code->size;
    
    IR *intrm_repres = translateBinToIR (bin_code);
    IrDump (intrm_repres);

    fclose (code_file);

    X86_code *x86_code = translateIrToX86 (intrm_repres, bin_size);
    Codex86Dump (x86_code->buffer, x86_code->size);

    runCode (x86_code->buffer, x86_code->size);

    BinCodeDtor (bin_code);
    X86RepresentDtor (x86_code);
    IntrmRepresentDtor (intrm_repres);

    printf ("-- finishing\n\n");

    return 0;
}

//-----------------------------------------------------------------------------