#include <stdint.h>
#include <stddef.h>

#ifndef STRING_HPP
#define STRING_HPP

/*
 * Retrieve the length of a given string
 */
extern "C" size_t strlen(const char *string);

/*
 * Copies the string from the source address to the destionation address
 */
extern "C" size_t strcpy(char *stringDst, const char *stringSrc);

/*
 * Copies the first given number of bytes from the string at the source address
 * to the destination address
 */
extern "C" size_t strncpy(char *stringDst, const char *stringSrc, size_t bytesNo);

/*
 * Copies the first given number of bytes from the the source address
 * to the destination address
 */
extern "C" void* memcpy(void *destAddr, const void *srcAddr, size_t bytesNo);

/*
 * Overwrites the first given number of bytes at given address with
 * the given byte
 */
extern "C" void* memset(void *addr, int8_t byte, size_t bytesNo);

extern "C" int strcmp(char *str1, char *str2);

extern "C" int istrcmp(char *str1, char *str2);

extern "C" int strncmp(char *str1, char *str2, size_t n);

extern "C" int istrncmp(char *str1, char *str2, size_t n);

extern "C" char* strtok(char *str, const char *delim);

#endif /* STRING_HPP */
