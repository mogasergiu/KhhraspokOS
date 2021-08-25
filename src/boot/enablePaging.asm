[BITS 32]

PML4T equ 0x1000
PDPT0 equ 0x2003
PDT0 equ 0x3003
PT0 equ 0x4003

enablePaging:
    mov edi, PML4T
    mov cr3, edi
    xor eax, eax
    mov ecx, 0x1000
    rep stosd
    mov edi, cr3

    mov dword [edi], PDPT0
    add edi, 0x1000
    mov dword [edi], PDT0
    add edi, 0x1000
    mov dword [edi], PT0
    add edi, 0x1000

    mov ebx, 0x3
    mov ecx, 512

.setEntry:
    mov dword [edi], ebx
    add ebx, 0x1000
    add edi, 8
    loop .setEntry

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

    ret
