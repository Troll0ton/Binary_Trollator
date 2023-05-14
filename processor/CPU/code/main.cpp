#include "CPU/include/CPU.h"

//-----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
    Processor cpu = { 0 };

    if(processor_ctor (&cpu) == ERROR_CTOR)
    {
        return ERROR_CPU;
    }

    read_code_file (&cpu);

    cpu_dump (&cpu);

    handle_cmds (&cpu);

    processor_dtor (&cpu);

    return 0;
}

//-----------------------------------------------------------------------------

