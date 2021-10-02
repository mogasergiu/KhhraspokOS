#include <stdint.h>
#include <stddef.h>

#ifndef STRING_HPP
#define STRING_HPP

/*
 * Retrieve the length of a given string
 */
size_t strlen(const char *string);

/*
 * Copies the string from the source address to the destionation address
 */
size_t strcpy(char *stringDst, const char *stringSrc);

/*
 * Copies the first given number of bytes from the string at the source address
 * to the destination address
 */
size_t strncpy(char *stringDst, const char *stringSrc, size_t bytesNo);

/*
 * Copies the first given number of bytes from the the source address
 * to the destination address
 */
void* memcpy(void *destAddr, const void *srcAddr, size_t bytesNo);

/*
 * Overwrites the first given number of bytes at given address with
 * the given byte
 */
void* memset(void *addr, int8_t byte, size_t bytesNo);

int strcmp(char *str1, char *str2);

int istrcmp(char *str1, char *str2);

int strncmp(char *str1, char *str2, size_t n);

int istrncmp(char *str1, char *str2, size_t n);

char* strtok(char *str, const char *delim);

char* strcat(char *dst, char *src);

char* strcat(char *dst, char src);

char* strncat(char *dst, char *src, size_t n);

#endif /* STRING_HPP */
