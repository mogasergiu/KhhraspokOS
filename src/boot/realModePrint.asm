BITS 16

; Real Mode printing helper functions
realModePrint:
; Prints the string whose address has been passed into the `si` register
.puts:
.puts.iterateString:
    lodsb
    cmp al, 0
    je .puts.endIteration
    call .putc
    jmp .puts.iterateString

.puts.endIteration:
    ret

; Prints the character that was passed into the `al` register
.putc:
    ; TELETYPE OUTPUT
    xor bx, bx
    mov ah, 0xe
    int 0x10

    ret

