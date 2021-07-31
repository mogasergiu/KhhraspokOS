#include <portsIO.hpp>

using namespace PIO;

/*
 * Inline function to receive byte from given port
 * @port: port to retrieve byte from
 * @return: the byte received through given port
 */
inline uint8_t inByte(uint16_t port) {
    uint8_t returnValue;

    __asm__ __volatile__(
        "inb %1, %0"
        : "=a" (returnValue)
        : "Nd" (port)
    );

    return returnValue;
}

/*
 * Inline function to receive word from given port
 * @port: port to retrieve word from
 * @return: the word received through given port
 */
inline uint32_t inWord(uint16_t port) {
    uint32_t returnValue;

    __asm__ __volatile__(
        "inw %1, %0"
        : "=a" (returnValue)
        : "Nd" (port)
    );

    return returnValue;
}

/*
 * Inline function to receive dword from given port
 * @port: port to retrieve dword from
 * @return: the dword received through given port
 */
inline uint32_t inDword(uint16_t port) {
    uint32_t returnValue;

    __asm__ __volatile__(
        "inl %1, %0"
        : "=a" (returnValue)
        : "Nd" (port)
    );

    return returnValue;
}

/*
 * Inline function to send given byte through given port
 * @port: port to send byte through
 * @byte: the byte to send
 */
inline void outByte(uint16_t port, uint8_t byte) {
    __asm__ __volatile__(
        "outb %0, %1"
        :
        : "a" (byte), "Nd"(port)
    );
}

/*
 * Inline function to send given word through given port
 * @port: port to send word through
 * @word: the word to send
 */
inline void outWord(uint16_t port, uint16_t word) {
    __asm__ __volatile__(
        "outw %0, %1"
        :
        : "a" (word), "Nd" (port)
    );
}

/*
 * Inline function to send given dword through given port
 * @port: port to send dword through
 * @dword: the dword to send
 */
inline void outDword(uint16_t port, uint32_t dword) {
    __asm__ __volatile__(
        "outl %0, %1"
        :
        : "a" (dword), "Nd" (port)
    );
}

