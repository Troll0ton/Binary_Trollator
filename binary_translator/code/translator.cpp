#include "binary_translator/include/translator.h" 

//-----------------------------------------------------------------------------

#define CURR_CMD intrm_repres->buffer[i]

int double_printf (double *value)
{
    return printf("%lg\n\n", *value);
}

//-----------------------------------------------------------------------------

X86_represent *translateIrToX86 (Intrm_represent *intrm_repres, int bin_size)
{
    printf ("-- emit commands\n\n");

    X86_represent *x86_represent = (X86_represent*) calloc (1, sizeof (X86_represent));
    x86_represent->code = (char*) calloc (intrm_repres->size * tmp_size + PAGESIZE - 1, sizeof (char));
    x86_represent->prev_ptr = x86_represent->code;

    Jump_table jump_table = { 0 };
    jump_table.x86_pos = (char**) calloc (bin_size, sizeof (char*));
    
    // need to change allignment
    x86_represent->code = (char*)(((uint64_t) (x86_represent->code) + PAGESIZE - 1) & ~(PAGESIZE - 1));

    char *curr_pos = x86_represent->code;
                                    // there and in other places - SIZE of command
                                    //        |
                                    //        V        
    emitCmd (&curr_pos, (char*) &X86_MOV_R10, 2);
    emitAbsPtr (&curr_pos, (unsigned long long int) curr_pos);

    for(int i = 0; i < intrm_repres->size; i++)
    {
        double num = (double) CURR_CMD.imm_value;

        jump_table.x86_pos[CURR_CMD.bin_pos] = curr_pos;

        #define CMD_EMIT(cmd, code, ...)  \
        case cmd:                         \
        {                                 \
            __VA_ARGS__                   \
        }

        switch(CURR_CMD.command)
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
//                              EMITING
//-----------------------------------------------------------------------------

void emitCmd (char **code, char *cmd, int size)
{
    memcpy (*code, cmd, size);

    *code += size;
}

//-----------------------------------------------------------------------------

void emitPtr (char **code, unsigned int addr)
{
    memcpy (*code, &addr, size_ptr);

    *code += size_ptr;
}

//-----------------------------------------------------------------------------

void emitAbsPtr (char **code, unsigned long long int addr)
{
    memcpy (*code, &addr, size_abs_ptr);

    *code += size_abs_ptr;
}

//-----------------------------------------------------------------------------

void emitNum (char **code, double num)
{
    memcpy (*code, &num, size_num);

    *code += size_num;
}

//-----------------------------------------------------------------------------
//                              RUNNING
//-----------------------------------------------------------------------------

void runCode (char *code, int size)
{
    int mprotect_status = mprotect (code, size, PROT_READ | PROT_WRITE | PROT_EXEC);
    handleMprotextError (mprotect_status);

    char *prev_ptr = code;

    printf ("-- executing...       \n\n"
            "o o o o o o o o o o o \n\n");

    void (*execute_code) (void) = (void (*) (void)) (code);
    execute_code ();

    printf ("o o o o o o o o o o o  \n\n"
            "-- executing completed!\n\n");

    mprotect_status = mprotect (prev_ptr, size, PROT_READ | PROT_WRITE);
    handleMprotextError (mprotect_status);
}

//-----------------------------------------------------------------------------

void handleMprotextError (int mprotect_status)
{
    if(mprotect_status == mprotect_error)
    {
        printf ("-- ERROR, mprotect\n\n"
                "-- pagesize: %d\n\n", 
                PAGESIZE                );

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

    fprintf (dump_file, "- x86 translated code size: %d\n\n", size);

    for(int i = 0; i < size; i++)
    {
        unsigned int num = (unsigned int) (unsigned char) code[i];
        fprintf (dump_file, "%X ", num);
    }

    fclose (dump_file);
}

//-----------------------------------------------------------------------------
//                             CTORS AND DTORS
//-----------------------------------------------------------------------------

void X86RepresentDtor (X86_represent *x86_represent)
{
    free (x86_represent->prev_ptr);
    x86_represent->size = deleted;
}

//-----------------------------------------------------------------------------