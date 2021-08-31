#include <stddef.h>
#include <stdint.h>

#ifndef ACPI_HPP
#define ACPI_HPP

#define EBDA_START 0xe0000
#define EBDA_END 0xfffff

#define RSD_PTR 0x2052545020445352
#define MADT_PTR 0x43495041

namespace ACPI {
    struct Hdr {
        uint32_t signature;
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        uint8_t oemid[6];
        uint8_t oemtid[8];
        uint32_t oemRevision;
        uint32_t creatorId;
        uint32_t creatorRevision;
    } __attribute__((packed));

    struct GenericAddressStructure {
        uint8_t addsSpace;
        uint8_t bitWidth;
        uint8_t bitOffset;
        uint8_t accessSize;
        uint64_t addr;
    } __attribute__((packed));

    struct RSDT {
        Hdr h;
        uint32_t *ptrSDT;
    } __attribute__((packed));

    struct XSDT {
        Hdr h;
        uint64_t *ptrSDT;
    } __attribute__((packed));

    struct FADT {
        Hdr h;
        uint32_t firmwareCtrl;
        uint32_t fsdt;
        uint8_t  reserved;
        uint8_t  preferredPowerManagementProfile;
        uint16_t SCI_Interrupt;
        uint32_t SMI_CommandPort;
        uint8_t  acpiEnable;
        uint8_t  acpiDisable;
        uint8_t  S4BIOS_REQ;
        uint8_t  PSTATE_Control;
        uint32_t PM1aEventBlock;
        uint32_t PM1bEventBlock;
        uint32_t PM1aControlBlock;
        uint32_t PM1bControlBlock;
        uint32_t PM2ControlBlock;
        uint32_t PMTimerBlock;
        uint32_t GPE0Block;
        uint32_t GPE1Block;
        uint8_t  PM1EventLength;
        uint8_t  PM1ControlLength;
        uint8_t  PM2ControlLength;
        uint8_t  PMTimerLength;
        uint8_t  GPE0Length;
        uint8_t  GPE1Length;
        uint8_t  GPE1Base;
        uint8_t  CStateControl;
        uint16_t worstC2Latency;
        uint16_t worstC3Latency;
        uint16_t flushSize;
        uint16_t flushStride;
        uint8_t  dutyOffset;
        uint8_t  dutyWidth;
        uint8_t  dayAlarm;
        uint8_t  monthAlarm;
        uint8_t  century;
        uint16_t bootArchitectureFlags;
        uint8_t  reserved2;
        uint32_t flags;
        GenericAddressStructure resetReg;
        uint8_t  resetValue;
        uint8_t  reserved3[3];
        uint64_t X_FirmwareControl;
        uint64_t X_Dsdt;
        GenericAddressStructure X_PM1aEventBlock;
        GenericAddressStructure X_PM1bEventBlock;
        GenericAddressStructure X_PM1aControlBlock;
        GenericAddressStructure X_PM1bControlBlock;
        GenericAddressStructure X_PM2ControlBlock;
        GenericAddressStructure X_PMTimerBlock;
        GenericAddressStructure X_GPE0Block;
        GenericAddressStructure X_GPE1Block;
    } __attribute__((packed));

    struct MADT {
        Hdr header;
        uint32_t localApicAddr;
        uint32_t flags;
    } __attribute__((packed));

    void parseACPI();
}

#endif  /* ACPI_HPP */
