#include <acpi.hpp>

using namespace ACPI;

void parseAPIC(ACPI::MADT *madt) {
    //  TODO: figure out why MADT entries are empty
}

void ACPI::parseACPI() {
    uint8_t *ptr;

    for (ptr = (uint8_t*)EBDA_START; ptr < (uint8_t*)EBDA_END; ptr += 16) {
        if (*(uint64_t*)ptr == RSD_PTR) {
            break;
        }
    }

    if (ptr[15] == 0) {
        uint32_t rsdt = *((uint32_t*)(ptr + 16));

        uint32_t *rptr = (uint32_t*)(rsdt + sizeof(ACPI::Hdr));
        uint32_t *rptrEnd = (uint32_t*)((uint8_t*)(uintptr_t)rsdt +
                            ((ACPI::Hdr*)((uintptr_t)rsdt))->length);

        while (rptr < rptrEnd) {
            uint32_t addr = *rptr++;

            if (((ACPI::Hdr*)((uintptr_t)addr))->signature == MADT_PTR) {
                parseAPIC((ACPI::MADT*)((uintptr_t)addr));
            }
        }

    } else {
        uint32_t rsdt = *((uint32_t*)(ptr + 16));
        uint64_t xsdt = *((uint64_t*)(ptr + 24));

        if (xsdt) {
            uint64_t *xptr = (uint64_t*)(rsdt + sizeof(ACPI::Hdr));
            uint64_t *xptrEnd = (uint64_t*)((uint8_t*)(uintptr_t)rsdt +
                                ((ACPI::Hdr*)((uintptr_t)rsdt))->length);

            while (xptr < xptrEnd) {
                uint64_t addr = *xptr++;

                if (((ACPI::Hdr*)((uintptr_t)addr))->signature == MADT_PTR) {
                    parseAPIC((ACPI::MADT*)((uintptr_t)addr));
                }
            }

        } else {
            uint32_t *rptr = (uint32_t*)(rsdt + sizeof(ACPI::Hdr));
            uint32_t *rptrEnd = (uint32_t*)((uint8_t*)(uintptr_t)rsdt +
                                ((ACPI::Hdr*)((uintptr_t)rsdt))->length);

            while (rptr < rptrEnd) {
                uint32_t addr = *rptr++;

                if (((ACPI::Hdr*)((uintptr_t)addr))->signature == MADT_PTR) {
                    parseAPIC((ACPI::MADT*)((uintptr_t)addr));
                }
            }

        }
    }
}
