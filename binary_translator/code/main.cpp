#include "binary_translator/include/IR-parser.h"
#include "binary_translator/include/translator.h"
#include "binary_translator/include/executable_utils.h"
#include "binary_translator/include/common.h"

//----------------------------------------------------------------------------

int main ()
{
    FILE *log_file = fopen ("binary_translator/dump/log_file.txt", "w+");

    printf ("-- traslate to IR\n\n");

    FILE *guest_file = fopen ("processor/COMMON/files/code.bin", "rb");
    Guest_code *guest_code = readCodeFile (guest_file, log_file);
    int bin_size = guest_code->size;
    
    IR *ir = translateGuestToIr (guest_code, log_file);
    irDump (ir);

    guestCodeDtor (guest_code);
    fclose (guest_file);
    
    Host_code *Host_code = translateIrToHost (ir, bin_size);
    irDtor (ir);
                                                                    
    #ifdef ELF_MODE
    createELF (Host_code);
    #else
    runCode (Host_code->buffer, Host_code->size);
    #endif
    
    fclose (Host_code->dump_file);
    hostCodeDtor (Host_code);

    printf ("-- finishing\n\n");

    fclose (log_file);

    return 0;
}

//-----------------------------------------------------------------------------