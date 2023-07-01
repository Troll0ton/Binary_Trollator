#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/translator.h"
#include "binary_translator/include/executable_utils.h"
#include "binary_translator/include/common.h"

//----------------------------------------------------------------------------

int main ()
{
    printf ("-- trollating . . . . . . \n\n");

    FILE *log_file = fopen ("binary_translator/dump/log_file.txt", "w+");

    FILE *guest_file = fopen ("processor/COMMON/files/code.bin","rb");
    checkFilePtr (log_file);
    
    Guest_code *guest_code = readCodeFile (guest_file, log_file);
    
    int bin_size = guest_code->size;
    
    IR *ir = translateGuestToIr (guest_code, log_file);
    irDump (ir, log_file);

    guestCodeDtor (guest_code);
    fclose (guest_file);
    
    X64_code *X64_code = translateIrToX64 (ir, bin_size, log_file);
    irDtor (ir);

    printf ("-- trollating complete!\n\n");
                                                                    
    #ifdef ELF_MODE
    createELF (X64_code);
    #else
    runCode (X64_code->buffer, X64_code->size);
    #endif
    
    fclose (X64_code->dump_file);
    x64CodeDtor (X64_code);

    printf ("-- finishing\n\n");

    fclose (log_file);

    return 0;
}

//-----------------------------------------------------------------------------