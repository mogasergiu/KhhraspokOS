ORG 0x7e00

[BITS 32]

DATA_SEGMENT_OFFSET equ 0x10

startProtectedMode:
    ; Set up new segment registers
    mov ax, DATA_SEGMENT_OFFSET
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    ; Update stack pointers
    mov ebp, 0x200000
    mov esp, ebp

    call 0x100000

    jmp $

times 512 - ($ - $$) db 0

