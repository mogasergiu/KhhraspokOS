[BITS 64]

global _entry
extern main
extern exit

_entry:
    call main
    call exit
    jmp $

