#include <stddef.h>
#include <stdint.h>

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#define FILESYSTEM_PATH_MAX_SIZE 100
#define FILESYSTEM_DIR_MAX_SIZE 8

#define CURRENT_FILESYSTEMS_NUMBER 0x1
#define MAX_FILE_DESCRIPTORS_NUMBER 0x100
#define FILESYSTEM_NAME_MAX_SIZE 0x10

#define SEEK_SET 0x0
#define SEEK_CUR 0x1
#define SEEK_END 0x2

#define FAT16_SIGNATURE 0x29
#define FAT16_ENTRY_SIZE 0x02
#define FAT16_BAD 0xFF7
#define FAT16_UNUSED 0x00
#define FAT16_ITEM_DIRECTORY 0
#define FAT16_ITEM_FILE 1
#define FAT16_FILE_READ_ONLY 0x01
#define FAT16_FILE_HIDDEN 0x02
#define FAT16_FILE_SYSTEM 0x04
#define FAT16_FILE_VOLUME_LABEL 0x08
#define FAT16_FILE_SUBDIRECTORY 0x10
#define FAT16_FILE_ARCHIVED 0x20
#define FAT16_FILE_DEVICE 0x40
#define FAT16_FILE_RESERVED 0x80

namespace FILESYSTEM {

    struct parsedPath {
        char currDir[FILESYSTEM_DIR_MAX_SIZE];
        parsedPath *nextDir, *prevDir;
    };

    class Path {
        private:
            char cwd[FILESYSTEM_PATH_MAX_SIZE];
            char oldcwd[FILESYSTEM_PATH_MAX_SIZE];

        public:
            Path();

            size_t getCwd(char *buffer) const;

            size_t getOldcwd(char *buffer) const;

            size_t setCwd(char *new_cwd);

            parsedPath *parsePath(char *path) const;
    };

    struct FileStat {
        char name[9];
        char extension[4];
        uint8_t type;
        uint8_t creation10s;
        uint16_t creationTime;
        uint16_t creationDate;
        uint16_t lastAccess;
        uint32_t size;
    } __attribute__((packed));

    // this is to be inherited only
    class FS_TEMPLATE {
        public:
            parsedPath *path;

            virtual int fopenCallback(void *fd, char *filename,
                                        const char *mode);
            virtual int freadCallback(void *fd, void *buffer,
                                        size_t bytesCount) const;
            virtual int fseekCallback(void *fd, int offset,
                                        int whence) const;
            virtual int fcloseCallback(void *fd) const;
            virtual FileStat* fstatCallback(void *fd) const;
    };

    class VFS {
        private:
            FS_TEMPLATE *filesystems[CURRENT_FILESYSTEMS_NUMBER];
            void *fileDescriptors[MAX_FILE_DESCRIPTORS_NUMBER];

        public:
            int fopen(char *filename, const char *mode);
            int fread(int fd, void *buffer, size_t bytesCount) const;
            int fseek(int fd, int offset, int whence) const;
            int fclose(int fd) const;
            FileStat* fstat(int fd) const;
    };

    namespace FAT {
        struct exHeader16 {
            uint8_t BIOSDriveNum;
            uint8_t reserved1;
            uint8_t bootSignature;
            uint32_t volumeID;
            char volumeLabel[11];
            char fatTypeLabel[8];
        } __attribute__((packed));

        struct Header {
            uint8_t bootJmp[3];
            char oemName[8];
            uint16_t bytesPerSector;
            uint8_t sectorsPerCluster;
            uint16_t reservedSectorsCount;
            uint8_t tableCount;
            uint16_t rootEntryCount;
            uint16_t totalSectors16;
            uint8_t mediaType;
            uint16_t tableSize;
            uint16_t sectorsPerTrack;
            uint16_t headSideCount;
            uint32_t hiddenSectorsCount;
            uint32_t totalSectors32;

            union {
                exHeader16 hdr16;
            } xhdr;
        } __attribute__((packed));

        struct ItemHeader16 {
            char name[8];
            char extension[3];
            uint8_t attribute;
            uint8_t reserved;
            uint8_t creation10s;
            uint16_t creationTime;
            uint16_t creationDate;
            uint16_t lastAccess;
            uint16_t clusterHigh;
            uint16_t lastAccessTime;
            uint16_t lastAccessDate;
            uint16_t clusterLow;
            uint32_t size;    
        } __attribute__((packed));

        struct ItemHeader {
            union {
                ItemHeader16 hdr16;
            } hdr;
        } __attribute__((packed));

        struct Item {
            union {
                ItemHeader16 hdr16;
            } hdr;
            size_t itemsCount;
            size_t start, end;
            uint8_t type;
        } __attribute__((packed));

        class FAT16 : public FS_TEMPLATE {
            private:
                const char name[10] = "FAT16";

            public:
                Header hdr;
                Item root;

                FAT16();
                const char *getName() const;
                int fopenCallback(void *fd, char *filename, const char *mode);
                int freadCallback(void *fd, void *buffer,
                                    size_t bytesCount) const;
                int fseekCallback(void *fd, int offset, int whence) const;
                int fcloseCallback(void *fd) const;
                FileStat* fstatCallback(void *fd) const;
        };
    };

    struct FileDescriptor {
        int16_t idx;
        size_t nail, nailStart;
        FS_TEMPLATE *fs;
        union {
            FAT::ItemHeader16 *hdr16;
        } stat;
        bool eof;
    } __attribute__((packed));
};

extern FILESYSTEM::Path pathMgr;
extern FILESYSTEM::FAT::FAT16 fat16Handler;
extern FILESYSTEM::VFS vfsHandler;

#endif  /* FILESYSTEM_HPP */
