#include <stddef.h>
#include <stdint.h>

#ifndef STDIO_HPP
#define STDIO_HPP

#define CATCH_FIRE(condition, message) \
    do { \
        if ((condition)) { \
            kperror("[%s:%d]: %s\n",__FILE__,  __LINE__, (message)); \
            kperror(""); \
            __asm__ __volatile__( \
                "hlt" \
                : \
                : \
            ); \
        } \
    } while (0)

extern "C" void kputc(const char c);

extern "C" void kpwarn(const char *s);

extern "C" void kperror(const char *format, ...);

extern "C" void kprintf(const char *format, ...);

#endif  /* STDIO_HPP */
