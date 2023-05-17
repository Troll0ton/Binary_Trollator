#include "binary_translator/include/translator.h" 

//-----------------------------------------------------------------------------

#define CURR_CMD intrm_repres->buffer[i]

X86_represent *translateIrToX86 (Intrm_represent *intrm_repres)
{
    printf ("-- emit commands\n\n");

    X86_represent *x86_represent = (X86_represent*) calloc (1, sizeof (X86_represent));
    x86_represent->code = (char*) calloc (intrm_repres->size * 20 + PAGESIZE - 1, sizeof (char));
    
    x86_represent->code = (char*)(((uint64_t) (x86_represent->code) + PAGESIZE - 1) & ~(PAGESIZE - 1));

    char *curr_pos = x86_represent->code;

    emitCmd (&curr_pos, (char*) &X86_MOV_R10, 2 );
    emitAbsPtr (&curr_pos, (unsigned long long int) curr_pos);

    for(int i = 0; i < intrm_repres->size; i++)
    {
        long long int num = CURR_CMD.imm_value;

        #define CMD_EMIT(cmd, code, ...)  \
        case cmd:                         \
        {                                 \
            __VA_ARGS__                   \
            break;                        \
        }

        switch(intrm_repres->buffer[i].command)
        {
            #include "binary_translator/include/codegen/translations_x86.h"

            default:
                printf ("       - UKNOWN COMMAND!, pos %d\n", i);
                break;
        }
    }

    emitCmd (&curr_pos, (char*) &X86_RET, 1);

    x86_represent->size = curr_pos - x86_represent->code + 1;

    return x86_represent;
}

#undef CURR_CMD

//-----------------------------------------------------------------------------

void printLongLongInt ()
{
    printf ("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n\n");
}

//-----------------------------------------------------------------------------

void emitCmd (char **code, char *cmd, int size)
{
    memcpy (*code, cmd, size);

    *code += size;
}

//-----------------------------------------------------------------------------

void emitPtr (char **code, unsigned int addr)
{
    memcpy (*code, &addr, 4);

    *code += 4;
}

//-----------------------------------------------------------------------------

void emitAbsPtr (char **code, unsigned long long int addr)
{
    memcpy (*code, &addr, 8);

    *code += 8;
}

//-----------------------------------------------------------------------------

void emitNum (char **code, long long int num)
{
    memcpy (*code, &num, 8);

    *code += 8;
}

//-----------------------------------------------------------------------------

void runCode (char *code, int size)
{
    printf ("-- executing\n\n");

    printf ("--     execiting buffer size: %d\n\n", size);

    int mprotect_status = mprotect (code, size, PROT_READ | PROT_WRITE | PROT_EXEC);
    handleMprotextError (mprotect_status);

    void (*execute_code)(void) = (void (*) (void))(code);
    execute_code ();

    printf ("-- translated code done\n\n");

    mprotect_status = mprotect (code, size, PROT_READ | PROT_WRITE);
    handleMprotextError (mprotect_status);
}

//-----------------------------------------------------------------------------

void handleMprotextError (int mprotect_status)
{
    if(mprotect_status == -1)
    {
        printf ("-- ERROR, mprotect\n\n");

        printf ("-- pagesize: %d\n\n", PAGESIZE);

        printf ("-- ERROR CODES: %d - EINVAL\n"
                "                %d - EFAULT\n"
                "                %d - EACCES\n"
                "                %d - ENOMEM\n",
                EINVAL, EFAULT, EACCES, ENOMEM  );

        exit (errno);
    }
}

//-----------------------------------------------------------------------------

void Codex86Dump (char *code, int size)
{
    printf ("-- dump x86 code\n\n");

    FILE *dump_file = fopen ("binary_translator/dump/x86_dump.txt", "w+");

    fprintf (dump_file, 
            "-----------------------------------------------------------------------------\n"
            "                          X86 TRANSLATED BIN CODE                            \n"
            "-----------------------------------------------------------------------------\n\n");

    fprintf (dump_file, "- x86 translared code size: %d\n\n", size);

    for(int i = 0; i < size; i++)
    {
        unsigned int num = (unsigned int) (unsigned char) code[i];
        fprintf (dump_file, "%X ", num);
    }

    fclose (dump_file);
}

//-----------------------------------------------------------------------------