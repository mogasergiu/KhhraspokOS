global _start
global pitIRQHandler
global keyboardIRQHandler
global doNothingIRQHandler
global inByte
global inWord
global outByte
global outWord

extern kernelMain
extern pitIRQ
extern keyboardIRQ

; Export constructor and destructor sections
extern startCtors
extern endCtors
extern startDtors
extern endDtors

section .text
_start:
    mov ebx, startCtors
    jmp .ctorsIterate

.callConstructor:
    call [ebx]
    add ebx, 4
.ctorsIterate:
    cmp ebx, endCtors
    jb .callConstructor

    ; Enter the kernel
    call kernelMain

    mov  ebx, endDtors
    jmp  .dtorsIterate
.callDestructor:
    sub  ebx, 4
    call [ebx]
.dtorsIterate:
    cmp  ebx, startDtors
    ja   .callDestructor
 
    cli

; Halt after kernel exit
.hang:
    hlt
    jmp  .hang

pitIRQHandler:
    pushad
    cli
    call pitIRQ
    popad
    sti
    iret

doNothingIRQHandler:
    pushad
    cli
    push 0x20
    push 0x20
    call outByte
    add esp, 0x8
    popad
    sti
    iret

keyboardIRQHandler:
    pushad
    cli
    call keyboardIRQ
    popad
    sti
    iret

inByte:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8]
    in al, dx

    pop ebp
    ret

inWord:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp+8]
    in ax, dx

    pop ebp
    ret

outByte:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, al

    pop ebp
    ret

outWord:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, ax

    pop ebp
    ret

