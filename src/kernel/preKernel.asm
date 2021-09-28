global _start
global pitIRQHandler
global syscallISRHandler
global keyboardIRQHandler
global doNothingIRQHandler
global lapicTimerIRQHandler
global SpuriousInterruptHandler
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
global ret2User

extern kernelMain
extern pitIRQ
extern syscallISR
extern keyboardIRQ
extern APEntry
extern lapicTimerIRQ
extern dumpCPU
extern schedYield

; Export constructor and destructor sections
extern startCtors
extern endCtors
extern startDtors
extern endDtors

%macro IRQpop 0
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rdi
    pop rsi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
%endmacro

%macro IRQpush 0
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rsi
    push rdi
    push rdx
    push rcx
    push rbx
    push rax
%endmacro

section .text
_start:
    nop
    nop

.nopJmp:
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
    cli
    IRQpush
    call pitIRQ
    IRQpop
    sti
    iretq

SpuriousInterruptHandler:
    iretq

doNothingIRQHandler:
    IRQpush
    mov r15, rsp
    call dumpCPU
    IRQpop
    iretq

keyboardIRQHandler:
    IRQpush
    call keyboardIRQ
    IRQpop
    iretq

lapicTimerIRQHandler:
    IRQpush
    mov r15, rsp
    call lapicTimerIRQ
    IRQpop
    iretq

syscallISRHandler:
    IRQpush
    call syscallISR
    add rsp, 8  ; keep rax intact (syscall's return value)
    pop rbx
    pop rcx
    pop rdx
    pop rdi
    pop rsi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
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

ret2User:
    push rbp
    mov rbp, rsp

    push qword [rdi + 152] ; ss

    push qword [rdi + 144] ; rsp

    pushf
    pop rax
    or rax, 0x200
    push rax

    push qword [rdi + 128]
    
    push qword [rdi + 120]

    mov ax, [rdi + 152]
    mov ds, ax
    mov es, ax
    mov gs, ax

    mov rax, qword [rdi + 160]
    mov rdx, qword [rdi + 160]
    shr rdx, 32
    mov rcx, 0xc0000101

    wrmsr

    mov rax, qword [rdi + 168]
    mov rdx, qword [rdi + 168]
    shr rdx, 32
    mov rcx, 0xc0000100

    wrmsr

    mov r15, rdi
    mov rax, qword [r15]
    mov rbx, qword [r15 + 8]
    mov rcx, qword [r15 + 16]
    mov rdx, qword [r15 + 24]
    mov rdi, qword [r15 + 32]
    mov rsi, qword [r15 + 40]
    mov rbp, qword [r15 + 48]
    mov r8, qword [r15 + 56]
    mov r9, qword [r15 + 64]
    mov r10, qword [r15 + 72]
    mov r11, qword [r15 + 80]
    mov r12, qword [r15 + 88]
    mov r13, qword [r15 + 96]
    mov r14, qword [r15 + 104]
    mov r15, qword [r15 + 112]

    iretq

global acquireLock
global releaseLock

acquireLock:
    lock bts dword [rdi],0        ;Attempt to acquire the lock (in case lock is uncontended)
    jc .spin_with_pause
    ret
 
.spin_with_pause:
    pause                    ; Tell CPU we're spinning
    call schedYield
    test dword [rdi],1      ; Is the lock free?
    jnz .spin_with_pause     ; no, wait
    jmp acquireLock          ; retry
 
releaseLock:
    mov dword [rdi],0
    ret

