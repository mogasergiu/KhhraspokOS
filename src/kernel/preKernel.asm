global _start

extern kernelMain

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
