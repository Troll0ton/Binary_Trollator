#include "ASM/include/ASM.h"

//-----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
    Assembler Asm = { 0 };

    if(assembler_ctor (&Asm, argv) == ERROR_CTOR)
    {
        printf ("----ERROR Asm----");
    }

    assembling (&Asm);

    asm_dump (&Asm);

    assembler_dtor (&Asm);

    return 0;
}

//-----------------------------------------------------------------------------

