ORG 0x8000

[BITS 16]

APawakened:
    jmp APRealModeStart

apStack dw 0x7c00

; Include the Global Descriptor Table
%include "globalDescriptorTable.asm"

; Define macro's for Global Descriptor Table entries offsets
CODE_SEGMENT_OFFSET equ gdtStart.gdtCodeSegmentDescriptor - gdtStart
DATA_SEGMENT_OFFSET equ gdtStart.gdtDataSegmentDescriptor - gdtStart
PML4T equ 0x1000
IDTD equ 0x50a
LAPIC equ 0x500
ACTIVE_CPUS equ 0x509

APRealModeStart:
    cli
    cld
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, [apStack]

    lock sub word [apStack],0x100

    in al, 0x92
    or al, 0x02
    out 0x92, al

    lgdt [gdtDescriptor]

    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    jmp CODE_SEGMENT_OFFSET:APPRotectedModeStart

[BITS 32]

APPRotectedModeStart:
    mov ax, DATA_SEGMENT_OFFSET
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov ss, ax

    lgdt [gdtDescriptor64]

    mov edi, PML4T
    mov cr3, edi

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xc0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    jmp CODE_SEGMENT_OFFSET:APLongMode

[BITS 64]

APLongMode:
    mov rax, IDTD
    lidt [rax]

    mov rax, LAPIC
    mov rsi, [rax]
    xor rax, rax
    add rsi, 0x00f0
    mov rdi, rsi
    lodsd
    or eax, 0x100
    stosd

    sti

    mov rax, ACTIVE_CPUS
    lock inc byte [rax]

    jmp $

times 1024 - ($ - $$) db 0

