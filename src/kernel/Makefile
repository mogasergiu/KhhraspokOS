FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
        -falign-loops -fomit-frame-pointer \
        -finline-functions -Wno-unused-function -fno-builtin -Werror -Wall \
        -Wextra -Wno-unused-label -Wno-unused-parameter -nostdlib \
        -nostartfiles -nodefaultlibs -O0 -std=c++20 -mno-red-zone \
	-mgeneral-regs-only -m64 -mno-mmx -mno-sse -mno-sse2 -fstrength-reduce
KERNEL_SOURCES_PATH = ${KHH_HOME}/src/kernel
LIBK_PATH = $(KERNEL_SOURCES_PATH)/libk
INCLUDES = -I$(KERNEL_SOURCES_PATH)/includes -I$(LIBK_PATH)/includes
KERNEL_MAIN_SOURCE = $(KERNEL_SOURCES_PATH)/kernelMain.cpp
KERNEL_VIDEO_SOURCE = $(KERNEL_SOURCES_PATH)/video/VGA/textMode.cpp
KERNEL_INTERRUPTS_SOURCE = $(KERNEL_SOURCES_PATH)/interrupts/interrupts.cpp
KERNEL_PIC_SOURCE = $(KERNEL_SOURCES_PATH)/interrupts/PIC/pic.cpp
KERNEL_PIT_SOURCE = $(KERNEL_SOURCES_PATH)/interrupts/PIT/pit.cpp
KERNEL_APIC_SOURCE = $(KERNEL_SOURCES_PATH)/interrupts/APIC/apic.cpp
KERNEL_HEAP_SOURCE = $(KERNEL_SOURCES_PATH)/memory/heap/kpkheap.cpp
KERNEL_PAGING_SOURCE = $(KERNEL_SOURCES_PATH)/memory/paging/paging.cpp
KERNEL_ACPI_SOURCE = $(KERNEL_SOURCES_PATH)/memory/acpi/acpi.cpp
KERNEL_DISK_DRIVER_SOURCE = $(KERNEL_SOURCES_PATH)/drivers/ATA/ata.cpp
KERNEL_PCI_DRIVER_SOURCE = $(KERNEL_SOURCES_PATH)/drivers/PCI/pci.cpp
KERNEL_PATH_PARSER_SOURCE = $(KERNEL_SOURCES_PATH)/filesystem/pathParser.cpp
KERNEL_FAT16_SOURCE = $(KERNEL_SOURCES_PATH)/filesystem/FAT16/fat16.cpp
KERNEL_VFS_SOURCE = $(KERNEL_SOURCES_PATH)/filesystem/VFS/vfs.cpp
PRE_KERNEL_SOURCE = $(KERNEL_SOURCES_PATH)/preKernel.asm
KERNEL_MAIN_OBJECT = $(BUILD_PATH)/kernelMain.o
KERNEL_VIDEO_OBJECT = $(BUILD_PATH)/textMode.o
KERNEL_INTERRUPTS_OBJECT = $(BUILD_PATH)/interrupts.o
KERNEL_PIC_OBJECT = $(BUILD_PATH)/pic.o
KERNEL_PIT_OBJECT = $(BUILD_PATH)/pit.o
KERNEL_HEAP_OBJECT = $(BUILD_PATH)/heap.o
KERNEL_PAGING_OBJECT = $(BUILD_PATH)/paging.o
KERNEL_ACPI_OBJECT = $(BUILD_PATH)/acpi.o
KERNEL_DISK_DRIVER_OBJECT = $(BUILD_PATH)/ata.o
KERNEL_PCI_DRIVER_OBJECT = $(BUILD_PATH)/pci.o
KERNEL_PATH_PARSER_OBJECT = $(BUILD_PATH)/pathParser.o
KERNEL_APIC_OBJECT = $(BUILD_PATH)/apic.o
KERNEL_FAT16_OBJECT = $(BUILD_PATH)/fat16.o
KERNEL_VFS_OBJECT = $(BUILD_PATH)/vfs.o
PRE_KERNEL_OBJECT = $(BUILD_PATH)/preKernel.o
LIBK = $(BUILD_PATH)/libk.a
BUILD_PATH = ${KHH_HOME}/build
KERNEL_OBJECTS =$(PRE_KERNEL_OBJECT) \
                $(KERNEL_MAIN_OBJECT) \
                $(KERNEL_VIDEO_OBJECT) \
                $(KERNEL_INTERRUPTS_OBJECT) \
		$(KERNEL_PIC_OBJECT) \
		$(KERNEL_PIT_OBJECT) \
		$(KERNEL_HEAP_OBJECT) \
		$(KERNEL_PAGING_OBJECT) \
		$(KERNEL_DISK_DRIVER_OBJECT) \
		$(KERNEL_PATH_PARSER_OBJECT) \
		$(KERNEL_ACPI_OBJECT) \
		$(KERNEL_APIC_OBJECT) \
		$(KERNEL_FAT16_OBJECT) \
		$(KERNEL_VFS_OBJECT) \
		$(KERNEL_PCI_DRIVER_OBJECT)
KERNEL_LINKED = $(BUILD_PATH)/kernelLinked.o
KERNEL = ${KHH_HOME}/bin/kernel.bin
LINKER_SCRIPT = -T $(KERNEL_SOURCES_PATH)/linker.ld
LIBK_LINK = -L$(BUILD_PATH) -lk
LINKER_FLAGS = -g -relocatable


all: $(KERNEL)

$(KERNEL): $(LIBK) $(KERNEL_OBJECTS)
	${TARGET}-ld $(LINKER_FLAGS) $^ $(LIBK_LINK) -o $(KERNEL_LINKED)
	${TARGET}-g++ $(LINKER_SCRIPT) $(FLAGS) $(INCLUDES) \
	$^ $(LIBK_LINK) -fno-rtti -o $@

$(LIBK):
	cd $(LIBK_PATH); make

$(KERNEL_VIDEO_OBJECT): $(KERNEL_VIDEO_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_MAIN_OBJECT): $(KERNEL_MAIN_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_INTERRUPTS_OBJECT): $(KERNEL_INTERRUPTS_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_PIC_OBJECT): $(KERNEL_PIC_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_PIT_OBJECT): $(KERNEL_PIT_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_HEAP_OBJECT): $(KERNEL_HEAP_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_PAGING_OBJECT): $(KERNEL_PAGING_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_DISK_DRIVER_OBJECT): $(KERNEL_DISK_DRIVER_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_PCI_DRIVER_OBJECT): $(KERNEL_PCI_DRIVER_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_PATH_PARSER_OBJECT): $(KERNEL_PATH_PARSER_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_ACPI_OBJECT): $(KERNEL_ACPI_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_APIC_OBJECT): $(KERNEL_APIC_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_FAT16_OBJECT): $(KERNEL_FAT16_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(KERNEL_VFS_OBJECT): $(KERNEL_VFS_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(PRE_KERNEL_OBJECT): $(PRE_KERNEL_SOURCE)
	nasm -f elf64 -g $^ -o $@

.PHONY: clean

clean:
	rm -f $(KERNEL_OBJECTS) $(KERNEL) $(KERNEL_LINKED); \
	cd $(LIBK_PATH); make clean
