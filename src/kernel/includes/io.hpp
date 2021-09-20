#include <stdint.h>
#include <stddef.h>

#ifndef IO_HPP
#define IO_HPP

#define rdmsrl(eax, edx, ecx) { \
    do { \
        __asm__ __volatile__( \
            "rdmsr;" \
            : "=a" (eax), "=d" (edx) \
            : "c" (ecx) \
        ); \
    } while (0);

#define wrmsrl(eax, edx, ecx) { \
    do { \
        __asm__ __volatile__( \
            "wrmsr;" \
            : \
            : "a" (eax), "d" (edx), "c" (ecx)\
        ); \
    } while (0);

namespace PMIO {
    extern "C" uint8_t pInByte(uint16_t port);

    extern "C" uint16_t pInWord(uint16_t port);

    extern "C" uint32_t pInDWord(uint16_t port);

    extern "C" void pOutByte(uint16_t port, uint8_t byte);

    extern "C" void pOutWord(uint16_t port, uint16_t word);

    extern "C" void pOutDWord(uint16_t port, uint32_t dword);
};

namespace MMIO {
    extern "C" uint8_t mInByte(void *addr);

    extern "C" uint16_t mInWord(void *addr);

    extern "C" uint32_t mInDWord(void *addr);

    extern "C" void mOutByte(void *addr, uint8_t byte);

    extern "C" void mOutWord(void *addr, uint16_t word);

    extern "C" void mOutDWord(void *addr, uint32_t dword);
};

#endif  /* PORTS_IO_H */
