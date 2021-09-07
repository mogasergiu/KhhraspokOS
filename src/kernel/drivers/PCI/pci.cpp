#include <drivers.hpp>
#include <io.hpp>
#include <kstdio.hpp>

using namespace DRIVERS;

PCI::PCIDevType PCI::pciDevs[256];
uint8_t PCI::PCIDevicesCount;

static uint32_t makeAddress(uint8_t bus, uint8_t slot, uint8_t func,
                            uint8_t offset) {
    uint32_t addr;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
 
    addr = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    return addr;
}

uint8_t PCIinByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t addr = makeAddress(bus, slot, func, offset);

    PMIO::pOutDWord(PCI_CONFIG_ADDRESS, addr);
    return PMIO::pInByte(PCI_CONFIG_DATA + (offset & 0x03));
}

uint16_t PCIinWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t addr = makeAddress(bus, slot, func, offset);

    PMIO::pOutDWord(PCI_CONFIG_ADDRESS, addr);
    return PMIO::pInWord(PCI_CONFIG_DATA + (offset & 0x02));
}

uint32_t PCIinDWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t addr = makeAddress(bus, slot, func, offset);

    PMIO::pOutDWord(PCI_CONFIG_ADDRESS, addr);
    return PMIO::pInDWord(PCI_CONFIG_DATA);
}

void PCIoutByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset,
                uint8_t value) {
    uint32_t addr = makeAddress(bus, slot, func, offset);

    PMIO::pOutDWord(PCI_CONFIG_ADDRESS, addr);
    PMIO::pOutByte(PCI_CONFIG_DATA + (offset & 0x03), value);
}

void PCIoutWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset,
                uint16_t value) {
    uint32_t addr = makeAddress(bus, slot, func, offset);

    PMIO::pOutDWord(PCI_CONFIG_ADDRESS, addr);
    PMIO::pOutWord(PCI_CONFIG_DATA + (offset & 0x02), value);
}

void PCIoutDWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset,
                uint32_t value) {
    uint32_t addr = makeAddress(bus, slot, func, offset);

    PMIO::pOutDWord(PCI_CONFIG_ADDRESS, addr);
    PMIO::pOutDWord(PCI_CONFIG_DATA, value);
}

static inline void gatherPCIDevInfo(uint8_t bus, uint8_t device, uint8_t func) {
    PCI::pciDevs[PCI::PCIDevicesCount].deviceID = PCIinWord(bus, device, func,
                                        PCI_DEVICE_ID_OFFSET);

    PCI::pciDevs[PCI::PCIDevicesCount].classCode = PCIinByte(bus, device, func,
                                        PCI_CLASS_CODE_OFFSET);

    PCI::pciDevs[PCI::PCIDevicesCount].subclass = PCIinByte(bus, device, func,
                                        PCI_SUBCLASS_OFFSET);

    PCI::pciDevs[PCI::PCIDevicesCount].progIF = PCIinByte(bus, device, func,
                                        PCI_PROGIF_OFFSET);

    PCI::pciDevs[PCI::PCIDevicesCount].revisionID = PCIinByte(bus, device, func,
                                        PCI_REVISION_ID_OFFSET);

    PCI::pciDevs[PCI::PCIDevicesCount].hdrType = PCIinByte(bus, device, func,
                                        PCI_HEADER_TYPE_OFFSET);
}

static void printPCIDevice(PCI::PCIDevType *dev) {
    kprintf("Found PCI Device: %x (Device ID), %x (Vendor ID), %x (Class Code),"
           "%x (Subclass), %x (Prog IF), %x (Revision ID), %x (Header Type).\n",
           dev->deviceID,
           dev->vendorID,
           dev->classCode,
           dev->subclass,
           dev->progIF,
           dev->revisionID,
           dev->hdrType);
}

void PCI::printPCIDevices() {
    for (int i = 0; i < PCI::PCIDevicesCount; i++) {
        printPCIDevice(&PCI::pciDevs[i]);
    }
}

static void checkDevice(uint8_t bus, uint8_t device) {
    uint8_t func = 0;
 
    PCI::pciDevs[PCI::PCIDevicesCount].vendorID = PCIinWord(bus, device, func,
                                        PCI_VENDOR_ID_OFFSET);

    if(PCI::pciDevs[PCI::PCIDevicesCount].vendorID == 0xffff) {
        return;
    }

    gatherPCIDevInfo(bus, device, func);

    if((PCI::pciDevs[PCI::PCIDevicesCount].hdrType & 0x80) != 0) {
        for(func = 1; func < 8; func++) {
            PCI::PCIDevicesCount++;
            PCI::pciDevs[PCI::PCIDevicesCount].vendorID = PCIinWord(bus, device,
                                                    func, PCI_VENDOR_ID_OFFSET);

            if(PCI::pciDevs[PCI::PCIDevicesCount].vendorID != 0xffff) {
                gatherPCIDevInfo(bus, device, func);

            } else {                
                PCI::PCIDevicesCount--;
            }
        }
    }

    PCI::PCIDevicesCount++;
}

void PCI::parsePCI() {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t device; device  < 32; device++) {
            checkDevice((uint8_t)bus, device);
        }
    }
}
