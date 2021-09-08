#include <drivers.hpp>
#include <kpkheap.hpp>
#include <kstring.hpp>
#include <kstdio.hpp>

using namespace DRIVERS;

static inline void readLBA(size_t lba, size_t lbaNo, uint16_t* buffer)
{
    // Send 0xe0 for the "master" or 0xf0 for the "slave"
    // ORed with the highest 4 bits of the LBA to port 0x1f6
    PMIO::pOutByte(ATA_DRIVE_REG, (lba >> 24) | 0xe0);

    // Send the sectorcount to port 0x1f2
    PMIO::pOutByte(ATA_SECTOR_COUNT_REG, lbaNo);

    // Send the low 8 bits of the LBA to port 0x1f3
    PMIO::pOutByte(ATA_SECTOR_NUMBER_REG, (unsigned char)(lba & 0xff));

    // Send the next 8 bits of the LBA to port 0x1f4
    PMIO::pOutByte(ATA_CYLINDER_LOW_REG, (unsigned char)(lba >> 8));

    // Send the next 8 bits of the LBA to port 0x1f5
    PMIO::pOutByte(ATA_CYLINDER_HIGH_REG, (unsigned char)(lba >> 16));

    // Send the "READ SECTORS" command (0x20) to port 0x1f7
    PMIO::pOutByte(ATA_COMMAND_REG, 0x20);

    for (size_t count = 0; count < lbaNo; count++)
    {
        // Check if disk needs servicing
        uint8_t byte = PMIO::pInByte(ATA_STATUS_REG);
        while(!(byte & 0x08))
        {
            byte = PMIO::pInByte(ATA_STATUS_REG);
        }

        // Transfer 256 16-bit values, a uint16_t at a time, into your buffer
        // from I/O port 0x1f0
        for (int i = 0; i < 256; i++)
        {
            buffer[count] = PMIO::pInWord(ATA_DATA_REG);
            count++;
        }
    }
}

void DISK::readDisk(size_t &nail, size_t size, void *buffer) {
    size_t lba = nail / SECTOR_SIZE;
    size_t lbaNo = size / SECTOR_SIZE + 1;

    uint16_t *buffer2 = (uint16_t*)KPKHEAP::kpkZalloc(lbaNo * SECTOR_SIZE);
    CATCH_FIRE(buffer2 == NULL, "Could not allocate disk buffer!");

    readLBA(lba, lbaNo, buffer2);

    uint16_t lbaOffset = nail % SECTOR_SIZE;

    memcpy(buffer, (uint8_t*)buffer2 + lbaOffset, size);

    nail += size;

    KPKHEAP::kpkFree(buffer2);
}

