#include <stdlib.hpp>
#include <stdarg.h>
#include <stdio.hpp>
#include <string.hpp>
#include <syscall.hpp>

static size_t printHex(int64_t num, char *str) {
    strcat(str, '0');
    strcat(str, 'x');
    strcat(str, '0');

    size_t idx = strlen(str);

    if (num == 0) {
        return idx;
    }

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
        strcat(str + idx, hexChar);
        idx++;
    }

    return idx;
}

extern "C" void printf(const char *format, ...) {
    char str[200], *argStr;
    size_t idx = 0;
    va_list ap;
    va_start(ap, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 's':
                    argStr = va_arg(ap, char*);
                    strcat(str + idx, argStr);
                    idx += strlen(argStr);

                    break;

                case 'd':
                    itoa(va_arg(ap, int), 10, argStr);
                    strcat(str + idx, argStr);
                    idx += strlen(argStr);

                    break;

                case 'x':
                case 'X':
                    idx = printHex(va_arg(ap, int64_t), str);

                    break;

                case '%':
                    strcat(str + idx, '%');
                    idx++;

                    break;
            }

        } else {
            strcat(str + idx, format[i]);
            idx++;
        }
    }

    va_end(ap);

    str[idx] = 0;

    puts(str);
}


