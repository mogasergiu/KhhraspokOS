ORG 0x7c00

[BITS 16]

; Define macro's for Global Descriptor Table entries offsets
CODE_SEGMENT_OFFSET equ gdtStart.gdtCodeSegmentDescriptor - gdtStart
DATA_SEGMENT_OFFSET equ gdtStart.gdtDataSegmentDescriptor - gdtStart

; Add a temporary fake BIOS Parameter Block - will update later for FAT
fakeBPB:
    jmp short _startRealMode
    nop

times 33 db 0

; Real Mode Bootloader:
;       - Loads the other sectors into memory
;       - Represents the first memory sector (first 512 bytes)

_startRealMode:
    jmp 0x0:startRealMode

startRealMode:
    ; Clear Interrupts
    cli

    ; Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov bp, 0x7c00
    mov sp, bp

    ; Restore Interrupts
    sti

    ; Load second stage bootloader for protected mode
    call realModeDiskOperations.LBARead

    ; Clear Interrupts
    cli

    ; Enable the A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; place into the GDT register the address of the GDT descriptor
    lgdt [gdtDescriptor]

    ; set Protection Enable bit in Control Register 0
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; Restore Interrupts
    sti

    ; Jump to Protected Mode and update cs register
    jmp CODE_SEGMENT_OFFSET:0x7e00

; Include Real Mode Disk Operations helper functions
%include "realModeDiskOperations.asm"

; Include Real Mode Printing helper functions
%include "realModePrint.asm"

; Include the Global Descriptor Table
%include "globalDescriptorTable.asm"

; Specify padding up to boot signature
times 510 - ($ - $$) db 0

; Boot signature
dw 0xaa55

