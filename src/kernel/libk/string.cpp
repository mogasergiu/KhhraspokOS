#include <string.hpp>

/*
 * Retrieve the length of a given string
 * @string: the string whose length is to be retrieved
 * @return: length of given string
 */
size_t strlen(const char *string) {
    size_t length = 0;

    while (string[length]) {
        length++;
    }

    return length;
}

/*
 * Copies the string from the source address to the destionation address
 * @stringDst: the destination address
 * @stringSrc: the source address
 * @return: number of copied bytes
 */
size_t strcpy(char *stringDst, const char *stringSrc) {
    size_t length = 0;

    while (stringSrc[length]) {
        stringDst[length] = stringSrc[length];
        length++;
    }

    return length;
}

/*
 * Copies the first given number of bytes from the string at the source address
 * to the destination address
 * @stringDst: the destination address
 * @stringSrc: the source address
 * @bytesNo: number of bytes to be copied
 * @return: number of copied bytes
 */
size_t strncpy(char *stringDst, const char *stringSrc,size_t bytesNo) {
    size_t length = 0;

    for (; length < bytesNo; length++) {
        stringDst[length] = stringSrc[length];
    }

    return length;
}

/*
 * Copies the first given number of bytes from the the source address
 * to the destination address
 * @destAddr: the destination address
 * @srcAddr: the source address
 * @bytesNo: number of bytes to be copied
 * @return: number of copied bytes
 */
void* memcpy(void *destAddr, const void *srcAddr, size_t bytesNo) {
    char *_destAddr = (char*)destAddr;
    char *_srcAddr = (char*)srcAddr;

    for (size_t l = 0; l < bytesNo; l++) {
        _destAddr[l] = _srcAddr[l];
    }

    return destAddr;
}

/*
 * Overwrites the first given number of bytes at given address with
 * the given byte
 * @addr: the address to start overwriting the bytes at
 * @byte: the byte to overwrite with
 * bytesNo: how many bytes to overwrite
 */
void* memset(void *addr, int8_t byte, size_t bytesNo) {
    char *_addr = (char*)addr;

    for (size_t l = 0; l < bytesNo; l++) {
        _addr[l] = byte;
    }

    return addr;
}

