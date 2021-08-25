[BITS 16]

memRegionsCount equ 0x500
memoryMap equ 0x510
SMAP equ 0x534d4150

struc memMapEntry
    .baseAddr resq 1
    .length resq 1
    .type resd 1
    .attr resd 1
endstruc

getMemoryRegions
    xor ax, ax
    mov es, ax
    mov di, memoryMap
    mov edx, SMAP
    xor ebx, ebx
    mov si, memRegionsCount

.scanRegions:
    mov eax, 0xe820
    mov ecx, 0x18

    int 0x15

    test ebx, ebx
    je .doneScanning

    add di, 24
    inc byte [si]
    jmp .scanRegions

.doneScanning:
    ret

