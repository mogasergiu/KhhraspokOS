#include <interrupts.hpp>
#include <kstdio.hpp>
#include <syscall.hpp>
#include <stdarg.h>
#include <video.hpp>
#include <task.hpp>

using namespace INTERRUPTS;

INTERRUPTS::Interrupts intsHandler;

void APIC::initLAPICTimer() {
    static uint32_t ticks;

    if (ticks == 0) {
        apicHandler.LAPICout(LAPIC_TDCR, 0x3);

        apicHandler.LAPICout(LAPIC_TICR, 0xffffffff);

        apicHandler.LAPICout(LAPIC_LTR, LAPIC_TIMER_IDT_ENTRY);

        pitHandler.sleep(10);

        apicHandler.LAPICout(LAPIC_LTR, LAPIC_TIMER_DISABLE);

        ticks = 0xffffffff - apicHandler.LAPICin(LAPIC_TCCR);
    }


    apicHandler.LAPICout(LAPIC_LTR, LAPIC_TIMER_IDT_ENTRY |
                                    LAPIC_TIMER_PERIODIC_MODE);

    apicHandler.LAPICout(LAPIC_TDCR, 0x3);

    apicHandler.LAPICout(LAPIC_TICR, ticks);
}

static void wakeAPs() {
    for (uint8_t i = 0, id = apicHandler.getLAPICID();
        i < apicHandler.getLAPICCount(); i++) {
        uint8_t _id = apicHandler.getLAPICID(i);

        if (id != _id) {
            apicHandler.sendLAPICIPI(_id, LAPIC_ICR_INIT |
                                          LAPIC_ICR_PHYSICAL |
                                          LAPIC_ICR_ASSERT |
                                          LAPIC_ICR_EDGE |
                                          LAPIC_ICR_DEFAULT);
        }
    }

    pitHandler.sleep(10);

    for (uint8_t i = 0, id = apicHandler.getLAPICID();
        i < apicHandler.getLAPICCount(); i++) {
        uint8_t _id = apicHandler.getLAPICID(i);

        if (id != _id) {
            apicHandler.sendLAPICIPI(_id, 0x8 | LAPIC_ICR_SIPI |
                                          LAPIC_ICR_PHYSICAL |
                                          LAPIC_ICR_ASSERT |
                                          LAPIC_ICR_EDGE |
                                          LAPIC_ICR_DEFAULT);
        }
        pitHandler.sleep(10);
    }
}

// Constructor - sets up IDTR, zeroes out IDT and loads IDT (lidt)
void Interrupts::initInterrupts() {
    picHandler.initPIC();
    apicHandler.initAPICs();
    pitHandler.initPIT();

    for (uint32_t i = 0; i < sizeof(this->IDTDescriptors); i++) {
        ((char*)this->IDTDescriptors)[i] = 0;
    }

    this->IDTRDescriptor.limit = sizeof(this->IDTDescriptors) - 1;
    this->IDTRDescriptor.base = (uintptr_t) this->IDTDescriptors;

    uintptr_t *idtd = (uintptr_t*)IDTD_ADDR;
    *idtd = (uintptr_t)(&this->IDTRDescriptor);

    for (int i = 0; i < MAX_IDT_ENTRIES; i++) {
        this->setIDTEntry(i, IntHandlers::doNothingIRQHandler, INT_DEFAULT);
    }

    this->setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_TIMER, IntHandlers::pitIRQHandler,
                        INT_NMI);
    this->setIDTEntry(PIC1_IRQ0 + PIC1_IRQ_KEYBOARD,
                        IntHandlers::keyboardIRQHandler, INT_NMI);
    this->setIDTEntry(LAPIC_TIMER_IDT_ENTRY, IntHandlers::lapicTimerIRQHandler,
                        INT_NMI);
    this->setIDTEntry(SPURIOUS_INTERRUPT_IDT_ENTRY,
                        IntHandlers::SpuriousInterruptHandler, INT_DEFAULT);
    this->setIDTEntry(SYSCALL_IDT_ENTRY,
                        IntHandlers::syscallISRHandler, INT_SYSCALL);

    __asm__ __volatile__ (
        "movq %0, %%rsi;"
        "lidt 0(%%rsi);"
        "sti;"
        :
        : "r" (&this->IDTRDescriptor)
        : "rsi"
    );

    wakeAPs();

    apicHandler.initLAPICTimer();
}

extern "C" void IntHandlers::dumpCPU() {
    TASK::CtxRegisters *ctx;
    __asm__ __volatile__(
        "mov %%r15, %0;"
        : "=r"(ctx)
    );

    uint8_t id = apicHandler.getLAPICID();
    kprintf(      "LAPIC ID: %x\n"
                  "RAX: %x\n"
                  "RBX: %x\n"
                  "RCX: %x\n"
                  "RDX: %x\n"
                  "RDI: %x\n"
                  "RSI: %x\n"
                  "RBP: %x\n"
                  "R8: %x\n"
                  "R9: %x\n"
                  "R10: %x\n"
                  "R11: %x\n"
                  "R12: %x\n"
                  "R13: %x\n"
                  "R14: %x\n"
                  "R15: %x\n"
                  "CODE: %x\n"
                  "RIP: %x\n"
                  "CS: %x\n"
                  "FLAGS: %x\n"
                  "RSP: %x\n"
                  "SS: %x\n"
                  "GS: %x\n"
                  "FS: %x\n",
                   id,
                   ctx->rax,
                   ctx->rbx,
                   ctx->rcx,
                   ctx->rdx,
                   ctx->rdi,
                   ctx->rsi,
                   ctx->rbp,
                   ctx->r8,
                   ctx->r9,
                   ctx->r10,
                   ctx->r11,
                   ctx->r12,
                   ctx->r13,
                   ctx->r14,
                   ctx->r15,
                   ctx->rip,
                   ctx->cs,
                   ctx->flags,
                   ctx->rsp,
                   ctx->ss,
                   ctx->gs,
                   ctx->fs,
                   ((uint64_t*)(&ctx->fs))[1]
    );

    __asm__ __volatile__("hlt"::);
}

/*
 * Helper function to add an entry in the IDT
 * @number: desired interrupt number of the entry
 * @address: address of the function to be placed in the entry
 */
void Interrupts::setIDTEntry(uint32_t number, void (*address)(), uint8_t type) {
    IDTD *descriptor = this->IDTDescriptors + number;

    // Lower part of the interrupt function's offset address
    descriptor->offset1 = (uintptr_t)address & 0x0000ffff;

    // Code Segment from GDT
    descriptor->selector = 0x8;

    descriptor->ist = type;

    if (type == INT_SYSCALL) {
        descriptor->typeAttr = 0xee;

    } else {
        // P = 1b, DPL = 00b, S = 0b, type = 1110b
        descriptor->typeAttr = 0x8e;
    }

    // Higher part of the interrupt function's offset address
    descriptor->offset2 = ((uintptr_t)address & 0xffff0000) >> 0x10;

    descriptor->offset3 = ((uintptr_t)address & 0xffffffff00000000) >> 0x20;

    descriptor->zero32 = 0x0;
}

extern "C" long IntCallbacks::syscallISR(long arg1, ...) {
    long sysNo;

    __asm__ __volatile__(
        ""
        : "=a" (sysNo)
        :
    );

    va_list ap;
    va_start(ap, arg1);

    long ret = 0;

    switch (sysNo) {

        // SYS_PUTCH
        case SYS_PUTCH:
            vgaHandler.putChar((char)arg1, 15);

            break;

        case SYS_PUTS:
            ret = (long)vgaHandler.putString((char*)arg1, 15);

            break;

        case SYS_GETPID:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_GETPPID:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_GETTID:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_FORK:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_EXECVE:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, char*)),
                    "S" (va_arg(ap, char**))
            );

            break;

        case SYS_EXIT:
            taskMgr.endTask();

            break;

        case SYS_MMAP:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, void*)),
                "S" (va_arg(ap, size_t)), "d" (va_arg(ap, int))
            );

            break;

        case SYS_SCHED_YIELD:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo)
            );

            break;

        case SYS_SLEEP:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, uint64_t))
            );

            break;

        case SYS_KILL:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, uintptr_t))
            );

            break;

        case SYS_CREATE_THREAD:
                TASK::TaskHeader *task;
                uintptr_t edx, eax;

                __asm__ __volatile__(
                    "rdmsr;"
                    : "=a" (eax), "=d" (edx)
                    : "c" (0xc0000101)
                );

                task = (TASK::TaskHeader*)((edx <<  32) + eax);

                taskMgr.createTask(va_arg(ap, void (*)(int, char**)), 3,
                                    va_arg(ap, char*), task->PCB);

            break;

        case SYS_THREAD_JOIN:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, uintptr_t))
            );

            break;

        case SYS_MUNMAP:
            __asm__ __volatile__(
                "int $0x80;"
                : "=a"(ret)
                : "a" (sysNo), "D" (va_arg(ap, void*)),
                "S" (va_arg(ap, size_t))
            );

            break;

//        default:
//            printf("%x Syscall does not exit yet!\n");
    }

    va_end(ap);

    return ret;
}
