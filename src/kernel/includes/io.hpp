#include <stdint.h>
#include <stddef.h>

#ifndef PORTS_IO_H
#define PORTS_IO_H

namespace PMIO {
    extern "C" uint8_t pInByte(uint16_t port);

    extern "C" uint16_t pInWord(uint16_t port);

    extern "C" void pOutByte(uint16_t port, uint8_t byte);

    extern "C" void pOutWord(uint16_t port, uint16_t word);
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
