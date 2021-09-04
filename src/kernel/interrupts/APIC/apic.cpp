#include <acpi.hpp>
#include <interrupts.hpp>
#include <io.hpp>
#include <stdio.hpp>

using namespace INTERRUPTS;

INTERRUPTS::APIC apicHandler;

APIC::APIC() {
    this->lapicAddr = (uintptr_t*)LAPIC_ADDR;

    this->activeCPUs = (uint8_t*)ACTIVE_CPUS;
    *this->activeCPUs = 0;

    this->lapicCount = 0;
}

void APIC::parseMADT() {
    *this->lapicAddr = (uintptr_t)ACPI::madt->LAPICAddr;

    printf("LAPIC addr %x -> %x\n", this->lapicAddr, *this->lapicAddr);

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
                pwarn("Unknown MADT entry found, stopped parsing MADT!");

                printf("Found %d lapics", this->lapicCount);

                return;
        }
    }

   printf("Found %d lapics", this->lapicCount);
}
