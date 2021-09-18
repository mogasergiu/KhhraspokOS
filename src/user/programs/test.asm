[BITS 64]

global _entry


_entry:
    add rsp, 10000
    mov rax, 1
    mov rdi, str
    int 0x80
    jmp $

str: db "Hello from Userspace :D!"
