SOURCES = ${KHH_HOME}/src
BOOTLOADER_SOURCES_PATH = $(SOURCES)/boot
KERNEL_SOURCES_PATH = $(SOURCES)/kernel
USER_SOURCES_PATH = $(SOURCES)/user
BINARIES = ${KHH_HOME}/bin
BOOTLOADER = $(BINARIES)/boot/bootloader.bin
KERNEL = $(BINARIES)/kernel/kernel.bin
OS_BINARY = ${KHH_HOME}/KhhraspokOS.bin


all: $(OS_BINARY)

$(OS_BINARY): $(BOOTLOADER) $(KERNEL)
	rm -f $(OS_BINARY)
	dd if=$(BOOTLOADER) >> $@
	dd if=$(KERNEL) >> $@
	dd if=/dev/zero bs=1048576 count=16 >> $@
	cd $(USER_SOURCES_PATH); make; cd ${KHH_HOME}

$(BOOTLOADER): $(BOOTLOADER_SOURCES_PATH)
	cd $(BOOTLOADER_SOURCES_PATH); make; cd ${KHH_HOME}

$(KERNEL): $(KERNEL_SOURCES_PATH)
	cd $(KERNEL_SOURCES_PATH); make; cd ${KHH_HOME}

.PHONY: clean

clean:
	rm -f $(OS_BINARY)
	cd $(BOOTLOADER_SOURCES_PATH); make clean; cd ${KHH_HOME}
	cd $(KERNEL_SOURCES_PATH); make clean; cd ${KHH_HOME}
	cd $(USER_SOURCES_PATH); make clean; cd ${KHH_HOME}

