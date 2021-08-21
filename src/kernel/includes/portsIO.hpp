#include <stdint.h>
#include <stddef.h>

#ifndef PORTS_IO_H
#define PORTS_IO_H

namespace PIO {
    extern "C" uint8_t inByte(uint16_t port);

    extern "C" uint16_t inWord(uint16_t port);

    extern "C" void outByte(uint16_t port, uint8_t byte);

    extern "C" void outWord(uint16_t port, uint16_t word);
};

#endif  /* PORTS_IO_H */
