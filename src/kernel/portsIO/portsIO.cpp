#include <portsIO.hpp>

using namespace PIO;

/*
 * Function to receive byte from given port
 * @port: port to retrieve byte from
 * @return: the byte received through given port
 */
uint8_t PIO::inByte(uint16_t port) {
    uint8_t returnValue;

    __asm__ __volatile__(
        "in %1, %0"
        : "=a" (returnValue)
        : "Nd" (port)
    );

    return returnValue;
}

/*
 * Function to receive word from given port
 * @port: port to retrieve word from
 * @return: the word received through given port
 */
uint16_t PIO::inWord(uint16_t port) {
    uint16_t returnValue;

    __asm__ __volatile__(
        "in %1, %0"
        : "=a" (returnValue)
        : "Nd" (port)
    );

    return returnValue;
}

/*
 * Function to receive dword from given port
 * @port: port to retrieve dword from
 * @return: the dword received through given port
 */
uint32_t PIO::inDword(uint16_t port) {
    uint32_t returnValue;

    __asm__ __volatile__(
        "inl %1, %0"
        : "=a" (returnValue)
        : "Nd" (port)
    );

    return returnValue;
}

/*
 * Function to send given byte through given port
 * @port: port to send byte through
 * @byte: the byte to send
 */
void PIO::outByte(uint16_t port, uint8_t byte) {
    __asm__ __volatile__(
        "out %0, %1"
        :
        : "a" (byte), "Nd"(port)
    );
}

/*
 * Function to send given word through given port
 * @port: port to send word through
 * @word: the word to send
 */
void PIO::outWord(uint16_t port, uint16_t word) {
    __asm__ __volatile__(
        "out %0, %1"
        :
        : "a" (word), "Nd" (port)
    );
}

/*
 * Function to send given dword through given port
 * @port: port to send dword through
 * @dword: the dword to send
 */
void PIO::outDword(uint16_t port, uint32_t dword) {
    __asm__ __volatile__(
        "outl %0, %1"
        :
        : "a" (dword), "Nd" (port)
    );
}

