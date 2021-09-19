[BITS 64]

global _entry
extern main

_entry:
    call main
    jmp $

