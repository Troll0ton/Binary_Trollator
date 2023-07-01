#include "ASM/include/ASM.h"

//-----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
    printf ("-- assembling\n\n");

    Assembler Asm = { 0 };

    if(assembler_ctor (&Asm, argc, argv) == ERROR_CTOR)
    {
        printf ("ERROR (while assembling)\n\n");
    }

    assembling     (&Asm);
    asm_dump       (&Asm);
    assembler_dtor (&Asm);

    return 0;
}

//-----------------------------------------------------------------------------

