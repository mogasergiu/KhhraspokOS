[BITS 64]

global _entry


_entry:
    add rsp, 10000
printme:
    mov rax, 1
    mov rdi, str
    int 0x80
    jmp printme

str: db "1", 10
