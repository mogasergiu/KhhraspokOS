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

extern "C" void putc(const char c);

extern "C" void pwarn(const char *s);

extern "C" void perror(const char *format, ...);

extern "C" void printf(const char *format, ...);

#endif  /* STDIO_HPP */
