#include <stddef.h>
#include <stdint.h>
#include <io.hpp>

#ifndef DRIVERS_HPP
#define DRIVERS_HPP

#define ATA_PRIMARY_BUS_IO_PORT_BASE 0x1f0
#define ATA_DATA_REG (ATA_PRIMARY_BUS_IO_PORT_BASE)
#define ATA_ERROR_REG (ATA_PRIMARY_BUS_IO_PORT_BASE + 1)
#define ATA_FEATURES_REG (ATA_PRIMARY_BUS_IO_PORT_BASE + 1)
#define ATA_SECTOR_COUNT_REG (ATA_PRIMARY_BUS_IO_PORT_BASE + 2)
#define ATA_SECTOR_NUMBER_REG (ATA_PRIMARY_BUS_IO_PORT_BASE + 3)
#define ATA_CYLINDER_LOW_REG (ATA_PRIMARY_BUS_IO_PORT_BASE + 4)
#define ATA_CYLINDER_HIGH_REG (ATA_PRIMARY_BUS_IO_PORT_BASE + 5)
#define ATA_DRIVE_REG (ATA_PRIMARY_BUS_IO_PORT_BASE + 6)
#define ATA_STATUS_REG (ATA_PRIMARY_BUS_IO_PORT_BASE + 7)
#define ATA_COMMAND_REG (ATA_PRIMARY_BUS_IO_PORT_BASE + 7)

#define SECTOR_SIZE 512

#define ATA_PRIMARY_BUS_CONTROL_PORT_BASE 0x3f6
#define ATA_ALTERNATE_STATUS_REG (ATA_PRIMARY_BUS_CONTROL_PORT_BASE)
#define ATA_DEVICE_CONTROL_REG (ATA_PRIMARY_BUS_CONTROL_PORT_BASE)
#define ATA_DRIVE_ADDRESS_REG (ATA_PRIMARY_BUS_CONTROL_PORT_BASE + 1)

#define PCI_CONFIG_ADDRESS 0xcf8
#define PCI_CONFIG_DATA 0xcfc

#define PCI_GENERIC_HEADER 0x0
#define PCI_TO_PCI_BRIDGE_HEADER 0x1
#define PCI_TO_CARDBUS_BRIDGE_HEADER 0x2

#define PCI_VENDOR_ID_OFFSET 0x0
#define PCI_DEVICE_ID_OFFSET 0x2
#define PCI_COMMAND_OFFSET 0x4
#define PCI_STATUS_FIELD_OFFSET 0x6
#define PCI_REVISION_ID_OFFSET 0x8
#define PCI_PROGIF_OFFSET 0x9
#define PCI_SUBCLASS_OFFSET 0xa
#define PCI_CLASS_CODE_OFFSET 0xb
#define PCI_CACHE_LINE_SIZE_OFFSET 0xc
#define PCI_LATENCY_TIMER_OFFSET 0xd
#define PCI_HEADER_TYPE_OFFSET 0xe
#define PCI_BIST_OFFSET 0xf

#define PS2_PORT 0x64
#define PS2_ENABLE_KEYBOARD 0xae

#define KEYBOARD_RELEASED 0x80
#define KEYBOARD_INPUT_PORT 0x60
#define KEYBOARD_BUFFER_MAX_SIZE 0x100
#define KEYBOARD_CAPSLOCK_PRESSED 0x3a
#define KEYBOARD_RSHIFT_RELEASED 0xb6
#define KEYBOARD_LSHIFT_RELEASED 0xaa
#define KEYBOARD_RSHIFT_PRESSED 0x36
#define KEYBOARD_LSHIFT_PRESSED 0x2a
#define KEYBOARD_ENTER_PRESSED 0x1c
#define KEYBOARD_SPACE_PRESSED 0x39

namespace DRIVERS {

    namespace DISK {
        void readDisk(size_t nail, size_t size, void* buffer);
    };

    namespace PCI {
        struct PCIDevType {
            uint16_t deviceID;
            uint16_t vendorID;
            uint8_t classCode;
            uint8_t subclass;
            uint8_t progIF;
            uint8_t revisionID;
            uint8_t hdrType;
        } __attribute__((packed));

        uint8_t PCIinByte(uint8_t bus, uint8_t slot, uint8_t fun,
                            uint8_t offset);

        uint16_t PCIinWord(uint8_t bus, uint8_t slot, uint8_t fun,
                            uint8_t offset);

        uint32_t PCIinDWord(uint8_t bus, uint8_t slot, uint8_t fun,
                            uint8_t offset);

        void PCIoutByte(uint8_t bus, uint8_t slot, uint8_t fun, uint8_t offset,
                        uint8_t value);

        void PCIoutWord(uint8_t bus, uint8_t slot, uint8_t fun, uint8_t offset,
                        uint16_t value);

        void PCIoutWord(uint8_t bus, uint8_t slot, uint8_t fun, uint8_t offset,
                        uint32_t value);

        extern PCIDevType pciDevs[256];

        extern uint8_t PCIDevicesCount;

        void printPCIDevices();

        void __attribute__((constructor)) parsePCI();
    };

    namespace PS2 {

        namespace KEYBOARD {

            extern char buffer[KEYBOARD_BUFFER_MAX_SIZE];

            void __attribute__((constructor)) initKeyboard();

            size_t readKeyboard(char *buf);
        }
    }
};

#endif  /* DRIVERS_HPP */
