#include <stddef.h>
#include <stdint.h>

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#define FILESYSTEM_PATH_MAX_SIZE 100
#define FILESYSTEM_DIR_MAX_SIZE 8

#define CURRENT_FILESYSTEMS_NUMBER 0x1
#define MAX_FILE_DESCRIPTORS_NUMBER 0x100
#define FILESYSTEM_NAME_MAX_SIZE 0x10

#define SEET_SET 0x0
#define SEET_CUR 0x1
#define SEET_END 0x2

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

    class VFS {
        public:
            virtual int fopen(const char *filename, const char *mode) const;
            void seekSet(int fd, uint8_t newSet) const;
    };

    struct FileDescriptor {
        uint8_t idx;
        size_t nail;
        VFS *fs;
    } __attribute__((packed));

    extern VFS *filesystems[CURRENT_FILESYSTEMS_NUMBER];
    extern FileDescriptor *fileDescriptors[MAX_FILE_DESCRIPTORS_NUMBER];

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

        struct Item {
            union {
                ItemHeader16 hdr16;
            } hdr;
            size_t itemsCount;
            size_t start, end;
            uint8_t type;
        } __attribute__((packed));

        class FAT16 : public VFS {
            private:
                const char name[10] = "FAT16";

            public:
                Header hdr;
                Item root;
                parsedPath *path;

                FAT16();
                const char *getName() const;
                int fopen(char *filename, const char *mode);
        };
    };
};

extern FILESYSTEM::Path pathMgr;
extern FILESYSTEM::FAT::FAT16 fat16Handler;

#endif  /* FILESYSTEM_HPP */
