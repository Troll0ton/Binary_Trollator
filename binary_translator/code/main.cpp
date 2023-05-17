#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/translator.h"
#include "binary_translator/include/common.h"

//-----------------------------------------------------------------------------

int main ()
{
    printf ("-- traslate to IR\n\n");

    FILE *code_file = fopen ("processor/COMMON/files/code.bin", "rb");

    Bin_code *bin_code = readCodeFile (code_file);
    Intrm_represent *intrm_repres = translateBinToIR (bin_code);
    IrDump (intrm_repres);

    fclose (code_file);

    X86_represent *x86_represent = translateIrToX86 (intrm_repres);
    Codex86Dump (x86_represent->code, x86_represent->size);

    runCode (x86_represent->code, x86_represent->size);

    printf ("-- finishing\n\n");

    return 0;
}

//-----------------------------------------------------------------------------