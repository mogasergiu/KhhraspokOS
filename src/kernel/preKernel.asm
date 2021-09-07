global _start
global pitIRQHandler
global keyboardIRQHandler
global doNothingIRQHandler
global lapicTimerIRQHandler
global pInByte
global pInWord
global pInDWord
global pOutByte
global pOutWord
global pOutDWord
global mInByte
global mInWord
global mInDWord
global mOutByte
global mOutWord
global mOutDWord

extern kernelMain
extern pitIRQ
extern keyboardIRQ
extern APEntry
extern lapicTimerIRQ

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
    nop
    nop

.nopJmp
    jmp short .BSPJumpToCtors

.APJumpToCpp:
    call APEntry

    hlt

.BSPJumpToCtors:
    mov word [.nopJmp],0x9090

    cli
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
    mov rdi, 0x500
    mov rdi, [rdi]
    add rdi, 0xb0
    xor rsi, rsi
    call mOutDWord
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

lapicTimerIRQHandler:
    IRQpush
    cli
    call lapicTimerIRQ
    IRQpop
    sti
    iretq

pInByte:
    push rbp
    mov rbp, rsp

    xor rax, rax
    mov rdx, rdi
    in al, dx

    leave
    ret

pInWord:
    push rbp
    mov rbp, rsp

    xor rax, rax
    mov rdx, rdi
    in ax, dx

    leave
    ret

pInDWord:
    push rbp
    mov rbp, rsp

    xor rax, rax
    mov rdx, rdi
    in eax, dx

    leave
    ret

pOutByte:
    push rbp
    mov rbp, rsp

    mov rax, rsi
    mov rdx, rdi
    out dx, al

    leave
    ret

pOutWord:
    push rbp
    mov rbp, rsp

    mov rax, rsi
    mov rdx, rdi
    out dx, ax

    leave
    ret

pOutDWord:
    push rbp
    mov rbp, rsp

    mov rax, rsi
    mov rdx, rdi
    out dx, eax

    leave
    ret

mInByte:
    push rbp
    mov rbp, rsp

    movzx rax, byte [rdi]

    leave
    ret

mInWord:
    push rbp
    mov rbp, rsp

    movzx rax, word [rdi]

    leave
    ret

mInDWord:
    push rbp
    mov rbp, rsp

    xor rax, rax
    mov eax, dword [rdi]

    leave
    ret

mOutByte:
    push rbp
    mov rbp, rsp

    mov byte [rdi], sil

    leave
    ret

mOutWord:
    push rbp
    mov rbp, rsp

    mov word [rdi], si

    leave
    ret

mOutDWord:
    push rbp
    mov rbp, rsp

    mov dword [rdi], esi

    leave
    ret

