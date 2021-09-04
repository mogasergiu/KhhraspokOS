#include <stdint.h>
#include <stddef.h>

#ifndef STRING_HPP
#define STRING_HPP

/*
 * Retrieve the length of a given string
 * @string: the string whose length is to be retrieved
 * @return: length of given string
 */
extern "C" size_t strlen(const char *string);

/*
 * Copies the string from the source address to the destionation address
 * @stringDst: the destination address
 * @stringSrc: the source address
 * @return: number of copied bytes
 */
extern "C" size_t strcpy(char *stringDst, const char *stringSrc);

/*
 * Copies the first given number of bytes from the string at the source address
 * to the destination address
 * @stringDst: the destination address
 * @stringSrc: the source address
 * @bytesNo: number of bytes to be copied
 * @return: number of copied bytes
 */
extern "C" size_t strncpy(char *stringDst, const char *stringSrc, size_t bytesNo);

/*
 * Copies the first given number of bytes from the the source address
 * to the destination address
 * @destAddr: the destination address
 * @srcAddr: the source address
 * @bytesNo: number of bytes to be copied
 * @return: number of copied bytes
 */
extern "C" void* memcpy(void *destAddr, const void *srcAddr, size_t bytesNo);

/*
 * Overwrites the first given number of bytes at given address with
 * the given byte
 * @addr: the address to start overwriting the bytes at
 * @byte: the byte to overwrite with
 * bytesNo: how many bytes to overwrite
 */
extern "C" void* memset(void *addr, int8_t byte, size_t bytesNo);

extern "C" int strcmp(const char *str1, const char *str2);

extern "C" int strncmp(const char *str1, const char *str2, size_t n);

extern "C" char* strtok(char *str, const char *delim);

#endif /* STRING_HPP */
