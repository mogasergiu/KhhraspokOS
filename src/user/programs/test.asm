[BITS 64]

global _entry

_entry:
    add rsp, 10000
    int 0x22
    jmp $

