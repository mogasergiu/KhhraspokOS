#include <elf.hpp>

unsigned long elf64Hash(const unsigned char *name) {
    unsigned long h = 0, g;

    while (*name) {
        h = (h << 4) + *name++;

        if ((g = (h & 0xf0000000))) {
            h ^= g >> 24;
        }

        h &= 0x0fffffff ; 
    }

    return h;
}
