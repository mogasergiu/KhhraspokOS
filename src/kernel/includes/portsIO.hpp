#include <stdint.h>
#include <stddef.h>

#ifndef PORTS_IO_H
#define PORTS_IO_H

/****************************************************************
 * This namespace contains every I/O Port related functionality *
 ****************************************************************/
namespace PIO {

    /*
     * Function to receive byte from given port
     * @port: port to retrieve byte from
     * @return: the byte received through given port
     */
    uint8_t inByte(uint16_t port);

    /*
     * Function to receive word from given port
     * @port: port to retrieve word from
     * @return: the word received through given port
     */
    uint16_t inWord(uint16_t port);

    /*
     * Function to receive dword from given port
     * @port: port to retrieve dword from
     * @return: the dword received through given port
     */
    uint32_t inDword(uint16_t port);

    /*
     * Function to send given byte through given port
     * @port: port to send byte through
     * @byte: the byte to send
     */
    void outByte(uint16_t port, uint8_t byte);

    /*
     * Function to send given word through given port
     * @port: port to send word through
     * @word: the word to send
     */
    void outWord(uint16_t port, uint16_t word);

    /*
     * Function to send given dword through given port
     * @port: port to send dword through
     * @dword: the dword to send
     */
    void outDword(uint16_t port, uint32_t dword);
}

#endif  /* PORTS_IO_H */
