#include <stdint.h>
#include <stddef.h>
#include <filesystem.hpp>

#ifndef STDLIB_HPP
#define STDLIB_HPP

extern "C" char* itoa(int num, int base);

extern "C" int kfopen(char *filename, const char *mode);

extern "C" int kfread(int fd, void *buffer, size_t bytesCount);

extern "C" int kfseek(int fd, int offset, int whence);

extern "C" int kfclose(int fd);

extern "C" FILESYSTEM::FileStat* kfstat(int fd);

#endif  /* STDLIB_HPP */
