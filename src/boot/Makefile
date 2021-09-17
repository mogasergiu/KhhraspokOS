BOOTLOADER_SOURCES_PATH = ${KHH_HOME}/src/boot
BINARIES = ${KHH_HOME}/bin/boot
STAGE_1_SOURCES = $(BOOTLOADER_SOURCES_PATH)/bootloader_stage_1.asm
STAGE_2_SOURCES = $(BOOTLOADER_SOURCES_PATH)/bootloader_stage_2.asm
AP_BOOT_SOURCES = $(BOOTLOADER_SOURCES_PATH)/ap_boot.asm
STAGE_1_BOOTLOADER = $(BINARIES)/bootloader_stage_1.bin
STAGE_2_BOOTLOADER = $(BINARIES)/bootloader_stage_2.bin
AP_BOOT = $(BINARIES)/ap_boot.bin
FINAL_BOOTLOADER = $(BINARIES)/bootloader.bin
FLAGS = -fbin

all: $(FINAL_BOOTLOADER)

$(FINAL_BOOTLOADER): $(STAGE_1_BOOTLOADER) $(STAGE_2_BOOTLOADER) $(AP_BOOT)
	rm -f $@
	dd if=$< >> $@
	dd if=$(word 2, $^) >> $@
	dd if=$(word 3, $^) >> $@

$(STAGE_1_BOOTLOADER): $(STAGE_1_SOURCES)
	nasm $(FLAGS) $< -o $@

$(STAGE_2_BOOTLOADER): $(STAGE_2_SOURCES)
	nasm $(FLAGS) $< -o $@

$(AP_BOOT): $(AP_BOOT_SOURCES)
	nasm $(FLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(STAGE_1_BOOTLOADER) $(STAGE_2_BOOTLOADER) $(FINAL_BOOTLOADER)
