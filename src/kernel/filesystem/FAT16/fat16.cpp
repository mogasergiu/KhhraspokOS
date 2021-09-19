#include <filesystem.hpp>
#include <drivers.hpp>
#include <kstdio.hpp>
#include <kstring.hpp>
#include <kpkheap.hpp>
#include <interrupts.hpp>

using namespace FILESYSTEM;

FAT::FAT16 fat16Handler;

static char *makeName(FAT::ItemHeader16 *item) {
    static char name[15];
    memset(name, 0, sizeof(name));
    int i, j;

    for (i = 0; item->name[i] != 0 && item->name[i] != ' ' && i < 8; i++) {
            name[i] = item->name[i];
    }

    if (item->extension[0] != 0 && item->extension[0] != ' ') {
        name[i++] = '.';

    } else {
        return name;
    }

    for (j = 0; item->extension[j] != 0 && item->extension[j] != ' ' && j < 3; j++) {
            name[i++] = item->extension[j];
    }

    return name;
}

static inline size_t findFATOffset(size_t cluster) {
    return fat16Handler.root.end +
            (cluster - 2) * fat16Handler.hdr.sectorsPerCluster
            *fat16Handler.hdr.bytesPerSector;
}

static inline uint16_t getFATEntry(size_t cluster) {
    size_t FATEntries = fat16Handler.hdr.reservedSectorsCount * SECTOR_SIZE;

    uint16_t entry;

    cli();
    DRIVERS::DISK::readDisk(FATEntries + cluster * 2, 2, &entry);
    sti();

    return entry;
}

static size_t findItem(size_t cluster, FAT::ItemHeader16 *item) {
    uint16_t entry = getFATEntry(cluster);

    while (entry != 0xffff) {
        switch (entry) {
            case 0xff8:
            case 0xfff:
                kpwarn("Last FIle Entry reached!\n");
                return 0;

            case FAT16_BAD:
                kpwarn("Corrupted File found!\n");
                return 0;

            case 0xff0:
            case 0xff6:
                kpwarn("Cannot read Reserved Sector!\n");
                return 0;

            case 0x0:
                kpwarn("File unallocated!\n");
                return 0;

            default:
                cluster = entry;
                entry = getFATEntry(entry);
        }
    }

    size_t nail = findFATOffset(cluster);

    return nail;
}

FAT::FAT16::FAT16() {
    size_t nail = 0;

    DRIVERS::DISK::readDisk(nail, sizeof(this->hdr), &this->hdr);

    char fat16Str[] = "FAT16";
    CATCH_FIRE(strncmp(this->hdr.xhdr.hdr16.fatTypeLabel, fat16Str, 5) != 0,
                "Could not initialize FAT16 Filesystem - no Volume Label!\n");

    CATCH_FIRE(this->hdr.xhdr.hdr16.bootSignature != 0x29,
                "Could not initialize FAT16 Filesystem - No FAT16 Signature!\n");

    nail = (this->hdr.tableCount * this->hdr.tableSize +
            this->hdr.reservedSectorsCount) * SECTOR_SIZE;

    this->root.start = nail;

    FAT::ItemHeader16 item;

    this->root.itemsCount = 0;

    do {

        DRIVERS::DISK::readDisk(nail, sizeof(item), &item);
        nail += sizeof(item);

        if ((uint8_t)item.name[0] == 0xe5) {
            continue;
        }

        if (item.name[0] != 0) {
            this->root.itemsCount++;
        }

    } while (item.name[0] != 0);

    this->root.end = this->root.start + this->hdr.rootEntryCount * 32;

    char rootStr[] = "/";
    strncpy(this->root.hdr.hdr16.name, rootStr, 1);
}

static void findInDir(size_t nail, char *name, FAT::ItemHeader16 *item) {
    char *nameItem;

    do {

        cli();
        DRIVERS::DISK::readDisk(nail, sizeof(*item), item);
        sti();
        nail += sizeof(*item);

        nameItem = makeName(item);

        if ((uint8_t)item->name[0] == 0xe5 || item->attribute == 15) {
            continue;
        }

        if (item->name[0] != 0) {
            if (istrcmp(name, nameItem) == 0) {
                return;
            }
        }

    } while (item->name[0] != 0);
}

int FAT::FAT16::fopenCallback(void *fd, char *filename, const char *mode) {
    this->path = pathMgr.parsePath(filename);
    this->path = this->path->nextDir;

    size_t nail = this->root.start;

    FAT::ItemHeader16 *item = (FAT::ItemHeader16*)KPKHEAP::kpkZalloc(sizeof(*item));
    CATCH_FIRE(item == NULL, "Could not allocate FAT16 Item");

    while (this->path != NULL) {
        findInDir(nail, this->path->currDir, item);

        if (item->name[0] == 0) {
            kpwarn("File or Directory not found!");

            return -1;
        }

        this->path = this->path->nextDir;
        if (this->path == NULL) {
            nail = findFATOffset((item->clusterHigh << 16) + item->clusterLow);
            break;
        }

        nail = findItem((item->clusterHigh << 16) + item->clusterLow, item);

        if (nail == 0) {
            KPKHEAP::kpkFree(item);

            return -1;
        }

        // nail = findFATOffset((item->clusterHigh << 16) + item->clusterLow);
    }

    ((FileDescriptor*)fd)->nail = ((FileDescriptor*)fd)->nailStart = nail;
    ((FileDescriptor*)fd)->fs = &fat16Handler;
    ((FileDescriptor*)fd)->stat.hdr16 = item;
    
    return 0;
}

int FAT::FAT16::freadCallback(void *fd, void *buffer, size_t bytesCount) const {
    FileDescriptor *fdPtr = (FileDescriptor*)fd;
    size_t clusterSize = 0x80 * 512;
    size_t entry = (fdPtr->stat.hdr16->clusterHigh << 16) + fdPtr->stat.hdr16->clusterLow;
    size_t nail = fdPtr->nailStart;

    for (size_t i = 0; entry != 0xffff;
        entry = getFATEntry(entry), nail = findFATOffset(entry), i += clusterSize) {
        cli();
        DRIVERS::DISK::readDisk(nail, clusterSize, (uint8_t*)buffer + i);
        sti();
    }

    return bytesCount;
}

int FAT::FAT16::fseekCallback(void *fd, int offset, int whence) const {
    FileDescriptor *fdPtr = (FileDescriptor*)fd;

    size_t nailEnd = fdPtr->nailStart + fdPtr->stat.hdr16->size;

    switch (whence) {
        case SEEK_SET:
            fdPtr->nail = fdPtr->nailStart;

            break;

        case SEEK_CUR:
            break;

        case SEEK_END:
            fdPtr->nail = nailEnd;

            break;
    }

    if ((fdPtr->nail + offset) > nailEnd ||
        (fdPtr->nail + offset) < fdPtr->nailStart) {
        return -1;
    }

    fdPtr->nail += offset;

    if (fdPtr->nail == nailEnd) {
        fdPtr->eof = true;
    }

    return 0;
}

int FAT::FAT16::fcloseCallback(void *fd) const {
    FileDescriptor *fdPtr = (FileDescriptor*)fd;

    KPKHEAP::kpkFree(fdPtr->stat.hdr16);

    return 0;
}

FileStat* FAT::FAT16::fstatCallback(void *fd) const {
    FileDescriptor *fdPtr = (FileDescriptor*)fd;

    FileStat *fst = (FileStat*)KPKHEAP::kpkZalloc(sizeof(*fst));

    memcpy(fst->name, fdPtr->stat.hdr16->name, sizeof(fst->name) - 1);

    fst->name[sizeof(fst->name) - 1] = 0;

    memcpy(fst->extension, fdPtr->stat.hdr16->extension, sizeof(fst->extension) - 1);

    fst->extension[sizeof(fst->extension) - 1] = 0;

    fst->type = fdPtr->stat.hdr16->attribute & FAT16_FILE_SUBDIRECTORY ?
                FAT16_ITEM_DIRECTORY :
                FAT16_ITEM_FILE;

    fst->creation10s = fdPtr->stat.hdr16->creation10s;

    fst->creationTime = fdPtr->stat.hdr16->creationTime;

    fst->creationDate = fdPtr->stat.hdr16->creationDate;

    fst->lastAccess = fdPtr->stat.hdr16->lastAccess;

    fst->size = fdPtr->stat.hdr16->size;

    return fst;
}

