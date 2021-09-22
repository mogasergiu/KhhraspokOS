#include <filesystem.hpp>
#include <kpkheap.hpp>
#include <kstdio.hpp>
#include <kstring.hpp>

using namespace FILESYSTEM;

#define STUB() \
    do { \
        kpwarn("This is stubbed, use the non-virtual instead\n"); \
        return 0; \
    } while (0);

#define DISK_FILESYSTEM (&fat16Handler)

VFS vfsHandler;

int FS_TEMPLATE::fopenCallback(void *fd, char *filename, const char *mode) {
    STUB();
}

int FS_TEMPLATE::freadCallback(void *fd, void *buffer, size_t bytesCount) const {
    STUB();
}

int FS_TEMPLATE::fseekCallback(void *fd, int offset, int whence) const {
    STUB();
}

int FS_TEMPLATE::fcloseCallback(void *fd) const {
    STUB();
}

FileStat* FS_TEMPLATE::fstatCallback(void *fd) const {
    STUB();
}

int VFS::fopen(char *filename, const char *mode) {
    FileDescriptor *fd = (FileDescriptor*)KPKHEAP::kpkZalloc(sizeof(*fd));
    CATCH_FIRE(fd == NULL, "Could not allocate File Descriptor");

    size_t filenameSize = strlen(filename) + 1;
    char *_filename = (char*)KPKHEAP::kpkZalloc(filenameSize);
    memcpy(_filename, filename, filenameSize);

    fd->idx = -1;

    for (int i = 0; i < MAX_FILE_DESCRIPTORS_NUMBER; i++) {
        if (this->fileDescriptors[i] == NULL) {
            this->fileDescriptors[i] = fd;
            fd->idx = i;

            break;
        }
    }

    if (fd->idx == -1) {
        KPKHEAP::kpkFree(fd);

        return -1;
    }

    fd->fs = DISK_FILESYSTEM;

    if (fd->fs->fopenCallback(fd, _filename, mode) != 0) {
        KPKHEAP::kpkFree(fd);
        KPKHEAP::kpkFree(_filename);

        return -1;
    }

    KPKHEAP::kpkFree(_filename);

    return fd->idx;
}

int VFS::fread(int fd, void *buffer, size_t bytesCount) const {
    FileDescriptor *fdPtr;

    if (fd >= MAX_FILE_DESCRIPTORS_NUMBER && !this->fileDescriptors[fd]) {
        return -1;

    }

    fdPtr = (FileDescriptor*)this->fileDescriptors[fd];

    if (buffer == NULL) {
        return -1;
    }

    return fdPtr->fs->freadCallback(fdPtr, buffer, bytesCount);
}

int VFS::fseek(int fd, int offset, int whence) const {
    FileDescriptor *fdPtr;

    if (fd >= MAX_FILE_DESCRIPTORS_NUMBER && !this->fileDescriptors[fd]) {
        return -1;

    }

    fdPtr = (FileDescriptor*)this->fileDescriptors[fd];
 
    return fdPtr->fs->fseekCallback(fdPtr, offset, whence);
}

int VFS::fclose(int fd) const {
    FileDescriptor *fdPtr;

    if (fd >= MAX_FILE_DESCRIPTORS_NUMBER && !this->fileDescriptors[fd]) {
        return -1;

    }

    fdPtr = (FileDescriptor*)this->fileDescriptors[fd];

    int ret = fdPtr->fs->fcloseCallback(fdPtr);

    KPKHEAP::kpkFree(fdPtr);

    return ret;
}

FileStat* VFS::fstat(int fd) const {
    FileDescriptor *fdPtr;

    if (fd >= MAX_FILE_DESCRIPTORS_NUMBER && !this->fileDescriptors[fd]) {
        return NULL;

    }

    fdPtr = (FileDescriptor*)this->fileDescriptors[fd];

    return fdPtr->fs->fstatCallback(fdPtr);
}
