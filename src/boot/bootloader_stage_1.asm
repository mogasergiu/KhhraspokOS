
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

; Prints the string whose address has been passed into the `si` register
realModePuts:
.iterateString:
    lodsb
    cmp al, 0
    je .endIteration
    call realModePutc
    jmp .iterateString

.endIteration:
    ret

; Prints the character that was passed into the `al` register
realModePutc:
    ; TELETYPE OUTPUT
    xor bx, bx
    mov ah, 0xe
    int 0x10

    ret

; Specify padding up to boot signature
times 510 - ($ - $$) db 0

; Boot signature
dw 0xAA55
