#include <filesystem.hpp>
#include <drivers.hpp>
#include <kstdio.hpp>
#include <kstring.hpp>
#include <kpkheap.hpp>

using namespace FILESYSTEM;

FAT::FAT16 fat16Handler;

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

        if ((uint8_t)item.name[0] == 0xe5) {
            continue;
        }

        if (item.name[0] != 0) {
            this->root.itemsCount++;
        }

    } while (item.name[0] != 0);

    nail = this->root.start;

    this->root.items.hdrs16 = (FAT::ItemHeader16*)
            KPKHEAP::kpkZalloc(this->hdr.rootEntryCount * sizeof(item));

    DRIVERS::DISK::readDisk(nail, this->hdr.rootEntryCount * sizeof(item),
                            this->root.items.hdrs16);

    this->root.end = nail;

    char rootStr[] = "/"
    strncpy(this->root.hdr16.name, rootStr, 1);
}

int FAT::FAT16::fopen(const char *filename, const char *mode) const {return 0;}
