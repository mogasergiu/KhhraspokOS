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
size_t strncpy(char *stringDst, const char *stringSrc, size_t bytesNo) {
    size_t length = 0;

    for (; length < bytesNo && stringSrc[length] != 0; length++) {
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

int strcmp(char *str1, char *str2) {
    uint32_t idx = 0;

    while ((str1[idx] != 0 && str2[idx] != 0) && str1[idx] == str2[idx]) {
        idx++;
    }

    if (str1[idx] == 0 && str2[idx] != 0) {
        return 1;

    } else if (str1[idx] != 0 && str2[idx] == 0) {
        return -1;

    } else {
        return str1[idx] - str2[idx];
    }
}

int istrcmp(char *str1, char *str2) {
    uint32_t idx = 0;

    while ((str1[idx] != 0 && str2[idx] != 0) && 
        (str1[idx] == str2[idx] || (str1[idx] + 32) == str2[idx] ||
        (str1[idx] - 32) == str2[idx])) {
        idx++;
    }

    if (str1[idx] == 0 && str2[idx] != 0) {
        return 1;

    } else if (str1[idx] != 0 && str2[idx] == 0) {
        return -1;

    } else {
        return str1[idx] - str2[idx];
    }
}

int strncmp(char *str1, char *str2, size_t n) {
    uint32_t idx = 0;

    while (idx < n && (str1[idx] != 0 && str2[idx] != 0) &&
            str1[idx] == str2[idx]) {
        idx++;
    }

    if (idx == n) {
        idx--;
    }

    if (str1[idx] == 0 && str2[idx] != 0) {
        return 1;

    } else if (str1[idx] != 0 && str2[idx] == 0) {
        return -1;

    } else {
        return str1[idx] - str2[idx];
    }
}

int istrncmp(char *str1, char *str2, size_t n) {
    uint32_t idx = 0;

    while (idx < n && (str1[idx] != 0 && str2[idx] != 0) && 
        (str1[idx] == str2[idx] || (str1[idx] + 32) == str2[idx] ||
        (str1[idx] - 32) == str2[idx])) {
        idx++;
    }

    if (idx == n) {
        idx--;
    }

    if (str1[idx] == 0 && str2[idx] != 0) {
        return 1;

    } else if (str1[idx] != 0 && str2[idx] == 0) {
        return -1;

    } else {
        return str1[idx] - str2[idx];
    }
}
char* strtok(char *str, const char *delim) {
    static char *start, *end;

    if (str != NULL) {
        start = str;

        if (*str == 0) {
            return NULL;
        }

    } else {
        if (end == NULL) {
            return NULL;
        }

        start = end + 1;
        str = end + 1;
    }

    while (*str) {
        for (int idx = 0; delim[idx] != 0; idx++) {
            if (*str == delim[idx]) {
                *str = 0;

                end = str;

                return start;
            }
        }

        str++;
    }

    end = NULL;

    return start;
}

char* strcat(char *dst, char *src) {
    size_t i;
    for (i = 0; src[i] != 0; i++) {
        dst[i] = src[i];
    }

    dst[i] = 0;

    return dst;
}

char* strcat(char *dst, char src) {
    size_t idx = strlen(dst);
    dst[idx] = src;
    dst[++idx] = 0;

    return dst;
}

char* strncat(char *dst, char *src, size_t n) {
    size_t i;
    for (i = 0; src[i] != 0 && i < n; i++) {
        dst[i] = src[i];
    }

    dst[i] = 0;

    return dst;
}
