#include <stddef.h>
#include <stdint.h>

#ifndef ELF_HPP
#define ELF_HPP

typedef uintptr_t Elf64_Addr;
typedef size_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef unsigned long Elf64_Xword;
typedef signed long Elf64_Sxword;

// File header
typedef  struct {
    unsigned  char e_ident[16]; /*  ELF  identification  */
    Elf64_Half e_type; /*  Object  file  type  */
    Elf64_Half e_machine; /*  Machine  type  */
    Elf64_Word e_version; /*  Object  file  version  */
    Elf64_Addr e_entry; /*  Entry  point  address  */
    Elf64_Off e_phoff; /*  Program  header  offset  */
    Elf64_Off e_shoff; /*  Section  header  offset  */
    Elf64_Word e_flags; /*  Processor-specific  flags  */
    Elf64_Half e_ehsize; /*  ELF  header  size  */
    Elf64_Half e_phentsize; /*  Size  of  program  header  entry  */
    Elf64_Half e_phnum; /*  Number  of  program  header  entries  */
    Elf64_Half e_shentsize; /*  Size  of  section  header  entry  */
    Elf64_Half e_shnum; /*  Number  of  section  header  entries  */
    Elf64_Half e_shstrndx; /*  Section  name  string  table  index  */
}__attribute__((packed)) Elf64_Ehdr;

// ELF Identification
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9
#define EI_NIDENT 16

// Object File Classes, e_ident[EI_CLASS]
#define ELFCLASS32 1
#define ELFCLASS64 2

// Data Encodings, e_ident[EI_DATA]
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

// Operating System and ABI Identifiers, e_ident[EI_OSABI]
#define ELFOSABI_SYSV 0
#define ELFOSABI_HPUX 1
#define ELFOSABI_STANDALONE 255

// Object File Types, e_type
#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
#define ET_LOOS 0xFE00
#define ET_HIOS 0xFEFF
#define ET_LOPROC 0xFF00
#define ET_HIPROC 0xFFFF


// Sections
typedef  struct {
    Elf64_Word sh_name; /*  Section  name  */
    Elf64_Word sh_type; /*  Section  type  */
    Elf64_Xword sh_flags; /*  Section  attributes  */
    Elf64_Addr sh_addr; /*  Virtual  address  in  memory  */
    Elf64_Off sh_offset; /*  Offset  in  file  */
    Elf64_Xword sh_size; /*  Size  of  section  */
    Elf64_Word sh_link; /*  Link  to  other  section  */
    Elf64_Word sh_info; /*  Miscellaneous  information  */
    Elf64_Xword sh_addralign; /*  Address  alignment  boundary  */
    Elf64_Xword sh_entsize; /*  Size  of  entries,  if  section  has  table  */
}__attribute__((packed)) Elf64_Shdr;

// Special Section Indices
#define SHN_UNDEF 0
#define SHN_LOPROC 0xFF00
#define SHN_HIPROC 0xFF1F
#define SHN_LOOS 0xFF20
#define SHN_HIOS 0xFF3F
#define SHN_ABS 0xFFF1
#define SHN_COMMON 0xFFF2

// Section Types, sh_type
#define SHT_NULL 0
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOOS 0x60000000
#define SHT_HIOS 0x6FFFFFFF
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7FFFFFFF

// Section Attributes, sh_flags  
#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKOS 0x0F000000
#define SHF_MASKPROC 0xF0000000

// Symbol table
typedef  struct {
    Elf64_Word st_name; /*  Symbol  name  */
    unsigned  char st_info; /*  Type  and  Binding  attributes  */
    unsigned  char st_other; /*  Reserved  */
    Elf64_Half st_shndx; /*  Section  table  index  */
    Elf64_Addr st_value; /*  Symbol  value  */
    Elf64_Xword st_size; /*  Size  of  object  (e.g.,  common)  */
}__attribute__((packed)) Elf64_Sym;

// Symbol Bindings
#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2
#define STB_LOOS 10
#define STB_HIOS 12
#define STB_LOPROC 13
#define STB_HIPROC 15

// Symbol Types 
#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_LOOS 10
#define STT_HIOS 12
#define STT_LOPROC 13
#define STT_HIPROC 15

// Relocations
typedef  struct {
    Elf64_Addr r_offset; /*  Address  of  reference  */
    Elf64_Xword r_info; /*  Symbol  index  and  type  of  relocation  */
}__attribute__((packed)) Elf64_Rel;

typedef  struct {
    Elf64_Addr r_offset; /*  Address  of  reference  */
    Elf64_Xword r_info; /*  Symbol  index  and  type  of  relocation  */
    Elf64_Sxword r_addend; /*  Constant  part  of  expression  */
}__attribute__((packed)) Elf64_Rela;

#define ELF64_R_SYM(i)((i) >> 32)
#define ELF64_R_TYPE(i)((i) & 0xffffffffL) 
#define ELF64_R_INFO(s, t)(((s) << 32) + ((t) & 0xffffffffL))

// Program header table
typedef  struct {
    Elf64_Word p_type; /*  Type  of  segment  */
    Elf64_Word p_flags; /*  Segment  attributes  */
    Elf64_Off p_offset; /*  Offset  in  file  */
    Elf64_Addr p_vaddr; /*  Virtual  address  in  memory  */
    Elf64_Addr p_paddr; /*  Reserved  */
    Elf64_Xword p_filesz; /*  Size  of  segment  in  file  */
    Elf64_Xword p_memsz; /*  Size  of  segment  in  memory  */
    Elf64_Xword p_align; /*  Alignment  of  segment  */
}__attribute__((packed)) Elf64_Phdr;

// Segment Types, p_type 
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOOS 0x60000000
#define PT_HIOS 0x6FFFFFFF
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7FFFFFFF

// Segment Attributes, p_flags
#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4
#define PF_MASKOS 0x00FF0000
#define PF_MASKPROC 0xFF00000

//Dynamic table
typedef  struct {
    Elf64_Sxword d_tag;
    union  {
        Elf64_Xword d_val;
        Elf64_Addr d_ptr;
    }  d_un;
}__attribute__((packed))  Elf64_Dyn;

struct ELF {
    Elf64_Ehdr ehdr;
    Elf64_Shdr shdr;
    Elf64_Sym sym;
    Elf64_Rel rel;
    Elf64_Rela rela;
    Elf64_Phdr phdr;
    Elf64_Dyn dyn;
}__attribute__((packed));

#endif  /*  ELF_HPP  */

