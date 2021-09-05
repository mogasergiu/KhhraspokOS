#include <acpi.hpp>
#include <interrupts.hpp>
#include <io.hpp>
#include <kstdio.hpp>

using namespace INTERRUPTS;

INTERRUPTS::APIC apicHandler;

APIC::APIC() {
    this->lapicAddr = (uintptr_t*)LAPIC_ADDR;

    this->activeCPUs = (uint8_t*)ACTIVE_CPUS;
    *this->activeCPUs = 1;

    this->lapicCount = 0;
}

void APIC::parseMADT() {
    *this->lapicAddr = (uintptr_t)ACPI::madt->LAPICAddr;

    for (uint8_t *entry = (uint8_t*)ACPI::madt + sizeof(*ACPI::madt);
        entry < (uint8_t*)ACPI::madt + ACPI::madt->hdr.length;) {
        switch (*entry) {
            case MADT_ENTRY_LAPIC:
                this->lapic[this->lapicCount] = (ACPI::LAPIC*)entry;

                entry += this->lapic[this->lapicCount++]->hdr.length;

                break;

            case MADT_ENTRY_IOAPIC:
                this->ioapic = (ACPI::IOAPIC*)entry;

                entry += this->ioapic->hdr.length;

                break;

            case MADT_ENTRY_IOAPICISO:
                this->ioapiciso[this->ioapicisoCount] =
                            (ACPI::IOAPICIntrSrcOverride*)entry;

                entry += this->ioapiciso[this->ioapicisoCount++]->hdr.length;

                break;

            case MADT_ENTRY_IOAPICNMIS:
                this->ioapicns[this->ioapicnsCount] = (ACPI::IOAPICNMISrc*)entry;

                entry += this->ioapicns[this->ioapicnsCount++]->hdr.length;

                break;

            case MADT_ENTRY_LAPICNMI:
                this->lapicn[this->lapicnCount] = (ACPI::LAPICNMI*)entry;

                entry += this->lapicn[this->lapicnCount++]->hdr.length;

                break;

            case MADT_ENTRY_LAPICAO:
                this->lapicao = (ACPI::LAPICAddrOverride*)entry;

                break;

            case MADT_ENTRY_Lx2APIC:
                this->lx2apic[this->lx2apicCount] = (ACPI::Lx2APIC*)entry;

                entry += this->lx2apic[this->lx2apicCount++]->hdr.length;

                break;

            default:
                kpwarn("Unknown MADT entry found, stopped parsing MADT!\n");

                return;
        }
    }
}

void APIC::IOAPICout(uint8_t reg, uint32_t value) {
    MMIO::mOutDWord((uint8_t*)((uintptr_t)this->ioapic->addr) + IOREGSEL, reg);
    MMIO::mOutDWord((uint8_t*)((uintptr_t)this->ioapic->addr) + IOWIN, value);
}

uint32_t APIC::IOAPICin(uint8_t reg) {
    MMIO::mOutDWord((uint8_t*)((uintptr_t)this->ioapic->addr) + IOREGSEL, reg);
    return MMIO::mInDWord((uint8_t*)((uintptr_t)this->ioapic->addr) + IOWIN);
}

void APIC::setIOAPICEntry(uint8_t idx, uint64_t addr) {
    this->IOAPICout(IOREDTBL + (idx << 1), (uint32_t)addr);

    this->IOAPICout(IOREDTBL + (idx << 1) + 1, (uint32_t)(addr >> 32));
}

uint8_t APIC::getIOAPICVersion() {
    return (uint8_t)this->IOAPICin(IOAPICVER);
}

uint8_t APIC::getIOAPICEntriesCount() {
    return (uint8_t)((this->IOAPICin(IOAPICVER) >> 16) & 0xff) + 1;
}

void APIC::LAPICout(uint32_t reg, uint32_t value) {
    MMIO::mOutDWord((uint8_t*)((uintptr_t)*this->lapicAddr) + reg, value);
}

uint32_t APIC::LAPICin(uint32_t reg) {
    return MMIO::mInDWord((uint8_t*)((uintptr_t)*this->lapicAddr) + reg);
}

uint8_t APIC::getLAPICID() {
    return this->LAPICin(LAPIC_ID) >> 24;
}

uint8_t APIC::getLAPICID(uint8_t id) {
    return this->lapic[id]->apicID;
}

void APIC::sendLAPICEOI() {
    MMIO::mOutDWord((uint8_t*)*this->lapicAddr + LAPIC_EOI, 0);
}

uint8_t APIC::getLAPICCount() {
    return this->lapicCount;
}

void APIC::sendLAPICIPI(uint8_t remoteLAPICID, uint32_t flags) {
    this->LAPICout(LAPIC_ICR1, remoteLAPICID << 24);
    this->LAPICout(LAPIC_ICR0, flags);

    while (this->LAPICin(LAPIC_ICR0) & LAPIC_ICR_PENDING);
}


void APIC::initAPICs() {
    this->parseMADT();

    this->LAPICout(LAPIC_SIVR, 0x100 | 0xff);

    this->LAPICout(LAPIC_LDR, 0x01000000);

    this->LAPICout(LAPIC_TPR, 0);

    this->LAPICout(LAPIC_DFR, 0xffffffff);

    uint8_t pitIRQ = PIC1_IRQ_TIMER;
    uint8_t keyboardIRQ = PIC1_IRQ_KEYBOARD;
    for (int i = 0; i < this->ioapicisoCount; i++) {
        if (this->ioapiciso[i]->IRQSrc == PIC1_IRQ_TIMER) {
            pitIRQ = this->ioapiciso[i]->glblSysIntr;

        } else if (this->ioapiciso[i]->IRQSrc == PIC1_IRQ_KEYBOARD) {
            keyboardIRQ = this->ioapiciso[i]->glblSysIntr;
        }
    }

    for (uint8_t entry = 0; entry < this->getIOAPICEntriesCount(); entry++) {
        if (entry == pitIRQ || entry == keyboardIRQ) {
            continue;
        }

        this->setIOAPICEntry(entry, 1 << 16);
    }

    this->setIOAPICEntry(pitIRQ, PIC1_IRQ0 + PIC1_IRQ_TIMER);

    this->setIOAPICEntry(keyboardIRQ, PIC1_IRQ0 + PIC1_IRQ_KEYBOARD);
}
