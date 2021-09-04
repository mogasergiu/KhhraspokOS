#include <stddef.h>
#include <stdint.h>

#ifndef ACPI_HPP
#define ACPI_HPP

#define EBDA_START 0xe0000
#define EBDA_END 0xfffff

#define IDTD_ADDR 0x50a
#define LAPIC_ADDR 0x500
#define ACTIVE_CPUS 0x509

#define RSD_PTR 0x2052545020445352

#define MADT_PTR 0x43495041
#define MADT_ENTRY_LAPIC 0x0
#define MADT_ENTRY_IOAPIC 0x1
#define MADT_ENTRY_IOAPICISO 0x2
#define MADT_ENTRY_IOAPICNMIS 0x3
#define MADT_ENTRY_LAPICNMI 0x4
#define MADT_ENTRY_LAPICAO 0x5
#define MADT_ENTRY_Lx2APIC 0x9

namespace ACPI {
    struct ACPIHdr {
        uint32_t signature;
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        uint8_t OEMID[6];
        uint8_t OEMTableID[8];
        uint32_t OEMevision;
        uint32_t creatorID;
        uint32_t creatorRevision;
    } __attribute__((packed));

    struct GenericAddressStructure {
        uint8_t addrSpace;
        uint8_t bitWidth;
        uint8_t bitOffset;
        uint8_t accessSize;
        uint64_t addr;
    } __attribute__((packed));

    struct RSDP {
        char signature[8];
        uint8_t checksum;
        char OEMId[6];
        uint8_t revision;
        uint32_t rsdtAddr;
        uint32_t Length;
        uint64_t xsdtAddr;
        uint8_t extendedChecksum;
        uint8_t reserved[3];
    } __attribute__ ((packed));

    struct RSDT {
        ACPIHdr hdr;
        uint32_t entries[0];
    } __attribute__((packed));

    struct XSDT {
        ACPIHdr hdr;
        uint64_t entries[0];
    } __attribute__((packed));

    struct FADT {
        ACPIHdr hdr;
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
        ACPIHdr hdr;
        uint32_t LAPICAddr;
        uint32_t flags;
    } __attribute__((packed));

    extern MADT *madt;

    struct LAPIC {
        struct {
            uint8_t type;
            uint8_t length;
        } __attribute__((packed)) hdr;
        uint8_t acpiID;
        uint8_t apicID;
        uint32_t flags;
    } __attribute__((packed));

    struct IOAPIC {
        struct {
            uint8_t type;
            uint8_t length;
        } __attribute__((packed)) hdr;
        uint8_t apicID;
        uint8_t reserved;
        uint32_t addr;
        uint32_t globalSysIntrBase;
    } __attribute__((packed));

    struct IOAPICIntrSrcOverride {
        struct {
            uint8_t type;
            uint8_t length;
        } __attribute__((packed)) hdr;
        uint8_t busSrc;
        uint8_t IRQSrc;
        uint32_t glblSysIntr;
        uint16_t flags;
    } __attribute__((packed));

    struct IOAPICNMISrc {
        struct {
            uint8_t type;
            uint8_t length;
        } __attribute__((packed)) hdr;
        uint8_t NMISrc;
        uint8_t reserved;
        uint16_t flags;
        uint32_t glblSysIntr;
    } __attribute__((packed));

    struct LAPICNMI {
        struct {
            uint8_t type;
            uint8_t length;
        } __attribute__((packed)) hdr;
        uint8_t acpiID;
        uint16_t flags;
        uint8_t LINT;
    } __attribute__((packed));

    struct LAPICAddrOverride {
        struct {
            uint8_t type;
            uint8_t length;
        } __attribute__((packed)) hdr;
        uint16_t reserved;
        uint64_t lapicAddr;
    } __attribute__((packed));

    struct Lx2APIC {
        struct {
            uint8_t type;
            uint8_t length;
        } __attribute__((packed)) hdr;
        uint16_t reserved;
        uint32_t Lx2ApicID;
        uint32_t flags;
        uint32_t acpiID;
    } __attribute__((packed));

    void parseACPI();
}

#endif  /* ACPI_HPP */
