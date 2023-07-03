#include "binary_translator/include/executable_utils.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                RUNNING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void runCode (char *code, int size)
{
    int mprotect_status = mprotect (code, size, PROT_READ | PROT_WRITE | PROT_EXEC);

    if(mprotect_status == MPROTECT_ERROR)
    {
        perror ("mprotect error:"); 
    }

    printf ("-- executing...        \n\n"
            "   o o o o o o o o o   \n\n");

    void (*execute_code) (void) = (void (*) (void)) (code);
    execute_code ();

    printf ("   o o o o o o o o o   \n\n"
            "-- executing completed!\n\n");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                                ELFING
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void programHeaderInit (char **curr_pos, int pos, Elf64_Word p_flags)
{
    Elf64_Phdr program_header = 
    {
        .p_type   = PT_LOAD,
        .p_flags  = p_flags,
        .p_offset = (Elf64_Addr) pos - LOAD_ADDR, 
        .p_vaddr  = (Elf64_Addr) pos, 
        .p_paddr  = (Elf64_Addr) pos, 
        .p_filesz = PAGE_SIZE, 
        .p_memsz  = PAGE_SIZE, 
        .p_align  = PAGE_SIZE, 
    };

    memcpy (*curr_pos, (const char*) &program_header, sizeof (Elf64_Phdr));
    *curr_pos += sizeof (Elf64_Phdr);
}
//-----------------------------------------------------------------------------

void createELF (X64_code *x64_code)
{
    printf ("Start creating ELF\n\n");

    FILE *executable = fopen ("executable", "w+");

    char *elf_buffer = (char*) calloc (1, TOTAL_SIZE);

    Elf64_Ehdr elf_header = 
    {
        .e_ident = 
        { 
            ELFMAG0, 
            ELFMAG1,  
            ELFMAG2,  
            ELFMAG3,   
            ELFCLASS64,  
            ELFDATA2LSB, 
            EV_CURRENT,    
            ELFOSABI_NONE,   
        },

        .e_type      = ET_EXEC,  
        .e_machine   = EM_X86_64, 
        .e_version   = EV_CURRENT, 
        .e_entry     = TEXT_ADDR, 
        .e_phoff     = sizeof (Elf64_Ehdr), 
        .e_ehsize    = sizeof (Elf64_Ehdr), 
        .e_phentsize = sizeof (Elf64_Phdr),  
        .e_phnum     = NUM_OF_SEGMENTS, 
    };

    char *curr_pos = elf_buffer;
    memcpy (elf_buffer, (char*) &elf_header, sizeof (Elf64_Ehdr));
    curr_pos += sizeof (Elf64_Ehdr);

    printf ("Created ELF header\n\n");
    
    programHeaderInit (&curr_pos, TEXT_ADDR,      PF_R | PF_X);
    programHeaderInit (&curr_pos, MEMORY_ADDRESS, PF_R | PF_W);
    programHeaderInit (&curr_pos, FUNCT_ADDR,     PF_R | PF_W | PF_X);

    printf ("Created program headers\n\n");

    memcpy (elf_buffer + TEXT_ADDR - LOAD_ADDR, x64_code->buffer, x64_code->size);
    writeInBinCode (elf_buffer + FUNCT_ADDR - LOAD_ADDR, "binary_translator/code/trolloprint.bin", x64_code);

    printf ("Pasted code\n\n");
    printf ("Total elf size: %d\n\n", TOTAL_SIZE);

    fwrite (elf_buffer, sizeof (char), TOTAL_SIZE, executable);
}

//-----------------------------------------------------------------------------

void writeInBinCode (char *pos, char *file_name, X64_code *x64_code)
{
    FILE *bin_file = fopen (file_name, "rb");
    int file_size = getFileSize (bin_file);

    fread (pos, sizeof (char), file_size, bin_file);

    fprintf (x64_code->dump_file, "%-25s | ", "FUNCT");

    for(int i = 0; i < file_size; i++)
    {
        uint32_t num = (uint32_t) (uint8_t) pos[i];
        fprintf (x64_code->dump_file, "%02X ", num);
    }

    fprintf (x64_code->dump_file, "\n");

    fclose (bin_file);
}

//-----------------------------------------------------------------------------