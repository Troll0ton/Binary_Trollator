#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/translator.h"
#include "binary_translator/include/executable_utils.h"
#include "binary_translator/include/common.h"

//----------------------------------------------------------------------------

int main ()
{
    FILE *log_file = fopen ("binary_translator/dump/log_file.txt", "w+");

    printf ("-- traslate to IR\n\n");

    FILE *code_file = fopen ("processor/COMMON/files/code.bin", "rb");
    Troll_code *bin_code = readCodeFile (code_file);
    int bin_size = bin_code->size;
    
    IR *ir = translateBinToIr (bin_code);
    IrDump (ir);

    BinCodeDtor (bin_code);
    fclose (code_file);
    
    X64_code *x64_code = translateIrToX64 (ir, bin_size);
    IrDtor (ir);
                                                                    
    #ifdef ELF_MODE
    createELF (x64_code);
    #else
    runCode (x64_code->buffer, x64_code->size);
    #endif
    
    fclose (x64_code->dump_file);
    x64CodeDtor (x64_code);

    printf ("-- finishing\n\n");

    fclose (log_file);

    return 0;
}

//-----------------------------------------------------------------------------