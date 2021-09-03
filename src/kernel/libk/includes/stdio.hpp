#include <stddef.h>
#include <stdint.h>

#ifndef STDIO_HPP
#define STDIO_HPP

#define CATCH_FIRE(condition, message) \
    do { \
        if ((condition)) { \
            perror("[%s:%d]: %s\n",__FILE__,  __LINE__, (message)); \
            perror(""); \
            __asm__ __volatile__( \
                "hlt" \
                : \
                : \
            ); \
        } \
    } while (0)

void putc(const char c);

void pwarn(const char *s);

void perror(const char *format, ...);

void printf(const char *format, ...);

#endif  /* STDIO_HPP */
