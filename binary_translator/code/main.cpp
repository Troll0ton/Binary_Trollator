#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/translator.h"
#include "binary_translator/include/executable_utils.h"
#include "binary_translator/include/common.h"

//----------------------------------------------------------------------------

int main ()
{
    FILE *log_file = fopen ("binary_translator/dump/log_file.txt", "w+");

    X64_code *X64_code = translateBinCode ("processor/COMMON/files/code.bin", log_file);
                                                                    
    #ifdef ELF_MODE
    createELF (X64_code);
    #else
    runCode (X64_code->buffer, X64_code->size);
    #endif
    
    x64CodeDtor (X64_code);

    return 0;
}

//-----------------------------------------------------------------------------