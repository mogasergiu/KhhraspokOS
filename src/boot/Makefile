BOOTLOADER_SOURCES_PATH = ${KHH_HOME}/src/boot
BINARIES = ${KHH_HOME}/bin
STAGE_1_SOURCES = $(BOOTLOADER_SOURCES_PATH)/bootloader_stage_1.asm \
                $(BOOTLOADER_SOURCES_PATH)/globalDescriptorTable.asm \
                $(BOOTLOADER_SOURCES_PATH)/realModeDiskOperations.asm \
                $(BOOTLOADER_SOURCES_PATH)/realModePrint.asm \
		$(BOOTLOADER_SOURCES_PATH)/realModeMemoryOps.asm
STAGE_2_SOURCES = $(BOOTLOADER_SOURCES_PATH)/bootloader_stage_2.asm
STAGE_1_BOOTLOADER = $(BINARIES)/bootloader_stage_1.bin
STAGE_2_BOOTLOADER = $(BINARIES)/bootloader_stage_2.bin
FINAL_BOOTLOADER = $(BINARIES)/bootloader.bin
FLAGS = -fbin

all: $(FINAL_BOOTLOADER)

$(FINAL_BOOTLOADER): $(STAGE_1_BOOTLOADER) $(STAGE_2_BOOTLOADER)
	rm -f $@
	dd if=$< >> $@
	dd if=$(word 2, $^) >> $@

$(STAGE_1_BOOTLOADER): $(STAGE_1_SOURCES)
	nasm $(FLAGS) $< -o $@

$(STAGE_2_BOOTLOADER): $(STAGE_2_SOURCES)
	nasm $(FLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(STAGE_1_BOOTLOADER) $(STAGE_2_BOOTLOADER) $(FINAL_BOOTLOADER)
