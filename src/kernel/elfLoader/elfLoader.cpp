#include <elf.hpp>
#include <memory.hpp>

static unsigned long elf64Hash(const unsigned char *name) {
    unsigned long h = 0, g;

    while (*name) {
        h = (h << 4) + *name++;

        if ((g = (h & 0xf0000000))) {
            h ^= g >> 24;
        }

        h &= 0x0fffffff ; 
    }

    return h;
}

static void* loadPHDR(void *elf, TASK::TaskHeader *task) {
    Elf64_Phdr *phdr = (Elf64_Phdr*)((uint8_t*)elf + ((Elf64_Ehdr*)elf)->e_phoff);
    size_t phdrCount = ((Elf64_Ehdr*)elf)->e_phnum;

    void *lastPaddr = NULL, *lastVaddr = (void*)USERSPACE_START_ADDR;
    size_t pg;
    for (size_t i = 0; i < phdrCount; i++) {
        switch (phdr[i].p_type) {
            case PT_NULL:
                break;

            case PT_LOAD:
                for (pg = (size_t)lastVaddr;
                        pg < (size_t)lastVaddr + phdr[i].p_memsz;
                        pg += PAGE_SIZE) {

                    lastPaddr = pageManager.reqPg();
                    if (lastPaddr == NULL) {
                        kpwarn("No more available pages!\n");

                        return NULL;
                    }

                    pageManager.mapPg((void*)pg, lastPaddr, PDE_P | PDE_R | PDE_U);
                }

                memcpy((void*)phdr[i].p_vaddr, (uint8_t*)elf + phdr[i].p_offset,
                        phdr[i].p_filesz);

                lastVaddr = (void*)pg;

                break;

            case PT_TLS:
                for (size_t pg = (size_t)lastVaddr;
                        pg < (size_t)lastVaddr + phdr[i].p_memsz;
                        pg += PAGE_SIZE) {

                    lastPaddr = pageManager.reqPg();
                    if (lastPaddr == NULL) {
                        kpwarn("No more available pages!\n");

                        return NULL;
                    }

                    pageManager.mapPg((void*)pg, lastPaddr, PDE_P | PDE_R | PDE_U);
                }

                memcpy((void*)phdr[i].p_vaddr, (uint8_t*)elf + phdr[i].p_offset,
                        phdr[i].p_filesz);

                task->PCB->tlsSize = phdr[i].p_memsz;

                lastVaddr = (void*)pg;

                task->TCB->ctxReg.fs = phdr[i].p_vaddr + phdr[i].p_memsz;

                ((uintptr_t*)task->TCB->ctxReg.fs)[0] = task->TCB->ctxReg.fs;

                task->PCB->ogTLS = KPKHEAP::kpkZalloc(task->PCB->tlsSize);
                memcpy(task->PCB->ogTLS, (uint8_t*)elf + phdr[i].p_offset,
                        phdr[i].p_filesz);

                break;

            default:
                kpwarn("PT not yet supported!\n");
        }
    }

    return lastVaddr;
}

void* loadELF(void *elf, TASK::TaskHeader *task) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)elf;

    if (ehdr->e_ident[EI_MAG0] != 0x7f || ehdr->e_ident[EI_MAG1] != 'E' ||
            ehdr->e_ident[EI_MAG2] != 'L' || ehdr->e_ident[EI_MAG3] != 'F') {
        kpwarn("Unknown ELF format\n");

        return NULL;
    }

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
        kpwarn("Unknown ELF Class - only ELF64 supported!\n");

        return NULL;
    }

    if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB) {
        kpwarn("ELF file not little endian!\n");

        return NULL;
    }

    if (ehdr->e_ident[EI_OSABI] != ELFOSABI_SYSV) {
        kpwarn("Only SysV ELF ABI supported!\n");

        return NULL;
    }

    if (ehdr->e_type != ET_EXEC) {
        kpwarn("ELF not executable!\n");

        return NULL;
    }

    task->TCB->ctxReg.rip = ehdr->e_entry;

    return loadPHDR(elf, task);
}

