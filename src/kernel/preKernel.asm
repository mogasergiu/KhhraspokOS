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

%macro IRQpush 0
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro IRQpop 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

section .text
_start:
    xor rbx, rbx
    mov ebx, startCtors
    jmp .ctorsIterate

.callConstructor:
    call [rbx]
    add rbx, 8
.ctorsIterate:
    cmp rbx, endCtors
    jb .callConstructor

    ; Enter the kernel
    call kernelMain

    mov  rbx, endDtors
    jmp  .dtorsIterate
.callDestructor:
    sub  rbx, 8
    call [rbx]
.dtorsIterate:
    cmp  rbx, startDtors
    ja   .callDestructor
 
    cli

; Halt after kernel exit
.hang:
    hlt
    jmp  .hang

pitIRQHandler:
    IRQpush
    cli
    call pitIRQ
    IRQpop
    sti
    iretq

doNothingIRQHandler:
    IRQpush
    cli
    mov rdi, 0x20
    mov rsi, 0x20
    call outByte
    IRQpop
    sti
    iretq

keyboardIRQHandler:
    IRQpush
    cli
    call keyboardIRQ
    IRQpop
    sti
    iretq

inByte:
    push rbp
    mov rbp, rsp

    xor rax, rax
    mov rdx, rdi
    in al, dx

    pop rbp
    ret

inWord:
    push rbp
    mov rbp, rsp

    xor rax, rax
    mov rdx, rdi
    in ax, dx

    pop rbp
    ret

outByte:
    push rbp
    mov rbp, rsp

    mov rax, rsi
    mov rdx, rdi
    out dx, al

    pop rbp
    ret

outWord:
    push rbp
    mov rbp, rsp

    mov rax, rsi
    mov rdx, rdi
    out dx, ax

    pop rbp
    ret


