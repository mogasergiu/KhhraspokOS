
ORG 0x7c00

BITS 16

; Real Mode Bootloader:
;       - Represents the first memory sector (first 512 bytes)

startRealMode:
    ; Clear Interrupts
    cli

    ; Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Restore Interrupts
    sti

    jmp $

; Specify padding up to boot signature
times 510 - ($ - $$) db 0

; Boot signature
dw 0xAA55
