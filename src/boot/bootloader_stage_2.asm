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

    ; Manually moving 8192 bytes at address 0x100000 where our kernel will be
    mov esi, 0x8000
    mov edi, 0x100000
    mov ecx, 0x2000
    rep movsb

    ; Update stack pointers
    mov ebp, 0x200000
    mov esp, ebp

    ; Jump to pre-kernel
    call 0x100000

    jmp $

times 512 - ($ - $$) db 0

