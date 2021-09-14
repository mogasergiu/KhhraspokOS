ORG 0x7e00

[BITS 32]

DATA_SEGMENT_OFFSET equ 0x10
CODE_SEGMENT_OFFSET equ 0x8

startProtectedMode: 
    ; Set up new segment registers
    mov ax, DATA_SEGMENT_OFFSET
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    call enablePaging

    lgdt [gdtDescriptor64]

    jmp CODE_SEGMENT_OFFSET:startLongMode

%include "enablePaging.asm"
%include "globalDescriptorTable.asm"

[BITS 64]

startLongMode:
    ; Manually moving bytes at address 0x100000 where our kernel will be
    mov rsi, 0x8400
    mov rdi, 0x100000
    mov rcx, 0x10000
    rep movsb

    ; Update stack pointers
    mov rbp, 0x200000
    mov rsp, rbp

    ; Jump to pre-kernel
    call 0x100000

    jmp $

times 512 - ($ - $$) db 0

