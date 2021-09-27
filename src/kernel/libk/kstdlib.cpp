#include <kstdlib.hpp>
#include <drivers.hpp>

static void reverse(char *str, int length) {
    --length;

    for (int i = 0; i < length; i++, length--) {
        str[i] ^= str[length];
        str[length] ^= str[i];
        str[i] ^= str[length];
    }
}
  
extern "C" char* itoa(int num, int base) {
    static char str[20];

    int i = 0;
    bool isNegative = false;
  
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
  
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
  
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
  
    if (isNegative)
        str[i++] = '-';
  
    str[i] = '\0';
  
    reverse(str, i);
  
    return str;
}

extern "C" int kfopen(char *filename, const char *mode) {
   return vfsHandler.fopen(filename, mode);
} 

extern "C" int kfread(int fd, void *buffer, size_t bytesCount) {
    return vfsHandler.fread(fd, buffer, bytesCount);
}

extern "C" int kfseek(int fd, int offset, int whence) {
    return vfsHandler.fseek(fd, offset, whence);
}

extern "C" int kfclose(int fd) {
    return vfsHandler.fclose(fd);
}

extern "C" FILESYSTEM::FileStat* kfstat(int fd) {
    return vfsHandler.fstat(fd);
}

extern "C" size_t fgets(char *buffer, size_t maxSize) {
    size_t len;

    while (!(len = DRIVERS::PS2::KEYBOARD::readKeyboard(buffer, maxSize)));

    return len;
}
