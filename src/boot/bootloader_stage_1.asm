
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

; This label marks the beginning of the Global Descriptor Table
gdtStart:

; the null descriptor is never referenced by the CPU, only in rare cases
.gdtNullSegmentDescriptor:
    dd 0x0
    dd 0x0

; used for code segment decriptor
.gdtCodeSegmentDescriptor:
    dw 0xffff  ; Limit field bits 0-15 - describes where segment ends

    dw 0  ; Base field bits 0-15 - describes where segment begins
    db 0  ; Base field bits 16-23

    ; Access byte:
    ; - Present bit set to 1 - must be 1 for all valid sectors
    ; - 2 Privilege bits set to 0 (ring 0)
    ; - Descriptor type bit S set to 1 (must be 1 for code or data segments)
    ; - Executable bit set to 1 - code must be executable
    ; - Direction Bit/Conforming bit set to 0 - only be executable in ring 0
    ; - Readable bit set to 1 (write permissions never allowed)
    ; - Accessed bit set to 0 - the CPU sets it to 1 when segment accessed
    db 10011010b

    ; Flags field nibble:
    ; - Granularity bit set to 1 - 4 Kib blocks (page granularity)
    ; - Size bit set to 1 - selector defines 32-bit protected mode
    ; - leave last 2 bits set to 0
    ; Followed by Limit field's 16-20 bits (lower nibble)
    ; So the Base field will be 0 and Limit field will be 0xfffff, this way the
    ; segment will span the full 4GiB address space
    db 11001111b

    db 0  ; Base field bits 24-31

; for the other registers
.gdtDataSegmentDescriptor:
    dw 0xffff  ; Limit field bits 0-15 - describes where segment ends

    dw 0  ; Base field bits 0-15 - describes where segment begins
    db 0  ; Base field bits 16-23

    ; Access byte:
    ; - Present bit set to 1 - must be 1 for all valid sectors
    ; - 2 Privilege bits set to 0 (ring 0)
    ; - Descriptor type bit S set to 1 (must be 1 for code or data segments)
    ; - Executable bit set to 0 - data must not be executable
    ; - Direction Bit/Conforming bit set to 0 - only be executable in ring 0
    ; - Writable bit set to 1 (readable permission always allowed)
    ; - Accessed bit set to 0 - the CPU sets it to 1 when segment accessed
    db 10010010b

    ; Flags field nibble:
    ; - Granularity bit set to 1 - 4 Kib blocks (page granularity)
    ; - Size bit set to 1 - selector defines 32-bit protected mode
    ; - leave last 2 bits set to 0
    ; Followed by Limit field's 16-20 bits (lower nibble)
    ; So the Base field will be 0 and Limit field will be 0xfffff, this way the
    ; segment will span the full 4GiB address space
    db 11001111b

    db 0  ; Base field bits 24-31

; This label marks the end of this Global Descriptor Table
gdtEnd:

; This label marks the structure to which the Global Descriptor Table Register
; (GDTR) should point to
gdtDescriptor:
    ; Size of the Global Descriptor Table minus 1
    dw gdtEnd - gdtStart - 1

    ; The starting address of the Global Descriptor Table
    dd gdtStart

; Specify padding up to boot signature
times 510 - ($ - $$) db 0

; Boot signature
dw 0xAA55
