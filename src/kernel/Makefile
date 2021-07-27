INCLUDES = -I ${KHH_HOME}/includes/
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
        -falign-loops -fstrength-reduce -fomit-frame-pointer \
        -finline-functions -Wno-unused-function -fno-builtin -Werror -Wall \
        -Wextra -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib \
        -nostartfiles -nodefaultlibs -O0 -Iinc
KERNEL_SOURCES_PATH = ${KHH_HOME}/src/kernel
KERNEL_SOURCES = $(KERNEL_SOURCES_PATH)/kernelMain.c
BUILD_PATH = ${KHH_HOME}/build
KERNEL_OBJECTS = $(BUILD_PATH)/kernelMain.o
KERNEL_LINKED = $(BUILD_PATH)/kernelLinked.o
KERNEL = ${KHH_HOME}/bin/kernel.bin
LINKER_SCRIPT = -T ${KHH_HOME}/linker.ld
LINKER_FLAGS = -g -relocatable


all: $(KERNEL)

$(KERNEL): $(KERNEL_OBJECTS)
	${TARGET}-ld $(LINKER_FLAGS) $^ -o $(KERNEL_LINKED)
	${TARGET}-gcc $(LINKER_SCRIPT) $(FLAGS) $(KERNEL_LINKED) -o $@

$(KERNEL_OBJECTS): $(KERNEL_SOURCES)
	${TARGET}-gcc $(FLAGS) $(INCLUDES) -c $^ -o $@

.PHONY: clean

clean:
	rm -f $(KERNEL_OBJECTS) $(KERNEL) $(KERNEL_LINKED)
