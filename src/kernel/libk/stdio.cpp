#include <video.hpp>
#include <stdlib.hpp>
#include <stdarg.h>

extern "C" void putc(const char c) {
    vgaHandler.putChar(c, 15);
}

extern "C" void pwarn(const char *s) {
    vgaHandler.putString(s, 14);
}

static void printHex(int64_t num) {
    vgaHandler.putString("0x", 15);

    int64_t hex = 0, remainder;
    char hexChar;

    for(int64_t hexByte = num; hexByte > 0; hexByte /= 0x10) {
        remainder = hexByte % 0x10;

        if(remainder < 10) {
	    remainder = remainder + 0x30;

        } else {
	    remainder = remainder + 0x37;
        }

        hex = hex * 100 + remainder;
    }

    for(int64_t hexByte = hex; hexByte > 0; hexByte = hexByte / 100) {
        hexChar = hexByte % 100;
        vgaHandler.putChar(hexChar, 15);
    }
}

extern "C" void perror(const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 's':
                    vgaHandler.putString(va_arg(ap, const char*), 12);
                    break;

                case 'd':
                    vgaHandler.putString(itoa(va_arg(ap, int), 10), 12);
                    break;

                case 'x':
                case 'X':
                    printHex(va_arg(ap, int64_t));
                    break;

                case '%':
                    vgaHandler.putChar('%', 12);
                    break;
            }

        } else {
            vgaHandler.putChar(format[i], 12);
        }
    }

    va_end(ap);
}

extern "C" void printf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 's':
                    vgaHandler.putString(va_arg(ap, const char*), 15);
                    break;

                case 'd':
                    vgaHandler.putString(itoa(va_arg(ap, int), 10), 15);
                    break;

                case 'x':
                case 'X':
                    printHex(va_arg(ap, int64_t));
                    break;

                case '%':
                    vgaHandler.putChar('%', 15);
                    break;
            }

        } else {
            vgaHandler.putChar(format[i], 15);
        }
    }

    va_end(ap);
}
