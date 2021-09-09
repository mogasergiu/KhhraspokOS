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

    DRIVERS::DISK::readDisk(FATEntries + cluster * 2, 2, &entry);

    return entry;
}

static FAT::ItemHeader16 *findItem(size_t cluster, FAT::ItemHeader16 *item) {
    uint16_t entry = getFATEntry(cluster);

    while (entry != 0xffff) {
        switch (entry) {
            case 0xff8:
            case 0xfff:
                kpwarn("Last FIle Entry reached!\n");
                return 0;
                break;

            case FAT16_BAD:
                kpwarn("Corrupted File found!\n");
                return 0;
                break;

            case 0xff0:
            case 0xff6:
                kpwarn("Cannot read Reserved Sector!\n");
                return 0;
                break;

            case 0x0:
                kpwarn("File unallocated!\n");
                return 0;
                break;

            default:
                cluster = entry;
                entry = getFATEntry(entry);
        }
    }

    size_t nail = findFATOffset(cluster);

    DRIVERS::DISK::readDisk(nail, sizeof(*item), item);

    return item;
}

int VFS::fopen(const char *filename, const char *mode) const {return 0;}

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

/*    this->root.items.hdrs16 = (FAT::ItemHeader16*)
            KPKHEAP::kpkZalloc(this->hdr.rootEntryCount * sizeof(item));

    DRIVERS::DISK::readDisk(nail, this->hdr.rootEntryCount * sizeof(item),
                            this->root.items.hdrs16);*/

    char rootStr[] = "/";
    strncpy(this->root.hdr.hdr16.name, rootStr, 1);
}

static void findInDir(size_t nail, char *name, FAT::ItemHeader16 *item) {
    char *nameItem;

    do {

        DRIVERS::DISK::readDisk(nail, sizeof(*item), item);
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

int FAT::FAT16::fopen(char *filename, const char *mode) {
    cli();
    this->path = pathMgr.parsePath(filename)->nextDir;

    size_t nail = this->root.start;

    FileDescriptor *fd = (FileDescriptor*)KPKHEAP::kpkZalloc(sizeof(*fd));
    CATCH_FIRE(fd == NULL, "Could not allocate File Descriptor");

    FAT::ItemHeader16 *item = (FAT::ItemHeader16*)KPKHEAP::kpkZalloc(sizeof(*item));
    CATCH_FIRE(item == NULL, "Could not allocate FAT16 Item");

    while (this->path != NULL) {
        findInDir(nail, this->path->currDir, item);

        if (item->name[0] == 0) {
            kpwarn("File or Directory not found!");

            return -1;
        }

        findItem((item->clusterHigh << 16) + item->clusterLow, item);

        nail = findFATOffset((item->clusterHigh << 16) + item->clusterLow);

        this->path = this->path->nextDir;
    }

    sti();

    return 0;
}
