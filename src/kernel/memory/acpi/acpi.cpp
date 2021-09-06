#include <acpi.hpp>
#include <interrupts.hpp>
#include <kstdio.hpp>

using namespace ACPI;

MADT* ACPI::madt;

static void parseRSDP(RSDP *rsdp) {
    if (rsdp->revision == 0) {
        RSDT *rsdt = (RSDT*)((uintptr_t)rsdp->rsdtAddr);

        for (int entry = 0; (uint8_t*)&rsdt->entries[entry] < (uint8_t*)rsdt + rsdt->hdr.length;
                entry++) {
            ACPIHdr *hdr = (ACPIHdr*)((uintptr_t)rsdt->entries[entry]);

            if (hdr->signature == MADT_PTR) {
                ACPI::madt = (MADT*)hdr;

                break;
            }
        }

    } else {
        RSDT *rsdt = (RSDT*)((uintptr_t)rsdp->rsdtAddr);
        XSDT *xsdt = (XSDT*)((uintptr_t)rsdp->xsdtAddr);

        if (xsdt != NULL) {
            kpwarn("Found XSDT instead of RSDT!");

            for (int entry = 0; (uint8_t*)&xsdt->entries[entry] < (uint8_t*)xsdt + xsdt->hdr.length;
                    entry++) {
                ACPIHdr *hdr = (ACPIHdr*)((uintptr_t)xsdt->entries[entry]);

                if (hdr->signature == MADT_PTR) {
                    ACPI::madt = (MADT*)hdr;

                    break;
                }
            }

        } else {
            for (int entry = 0; (uint8_t*)&rsdt->entries[entry] < (uint8_t*)rsdt + rsdt->hdr.length;
                    entry++) {
                ACPIHdr *hdr = (ACPIHdr*)((uintptr_t)rsdt->entries[entry]);

                if (hdr->signature == MADT_PTR) {
                    ACPI::madt = (MADT*)hdr;

                    break;
                }
            }
        }
    }
}

void __attribute__((constructor)) ACPI::parseACPI() {
    uint8_t *ptr;

    for (ptr = (uint8_t*)EBDA_START; ptr < (uint8_t*)EBDA_END; ptr += 16) {
        if (*(uint64_t*)ptr == RSD_PTR) {
            parseRSDP((RSDP*)ptr);
            break;
        }
    }

}
