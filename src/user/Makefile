FLAGS = -ffreestanding -falign-jumps -falign-functions -falign-labels \
        -falign-loops -fomit-frame-pointer \
        -finline-functions -Wno-unused-function -fno-builtin -Werror -Wall \
        -Wextra -Wno-unused-label -Wno-unused-parameter -nostdlib \
        -nostartfiles -nodefaultlibs -O0 -std=c++20 -mno-red-zone \
	-mgeneral-regs-only -m64 -mno-mmx -mno-sse -mno-sse2 -fstrength-reduce
USER_SOURCES_PATH = ${KHH_HOME}/src/user
USER_BUILD_PATH = ${KHH_HOME}/build/user
USER_BINARY_PATH = ${KHH_HOME}/bin/user
USER_ENTRY_SOURCE = $(USER_SOURCES_PATH)/entry.asm
USER_ENTRY_OBJECT = $(USER_BUILD_PATH)/entry.o
USER_SHELL_SOURCE = $(USER_SOURCES_PATH)/programs/shell.cpp
USER_SHELL_OBJECT = $(USER_BUILD_PATH)/shell.o
USER_SHELL_BINARY = $(USER_BINARY_PATH)/shell
LIBC_SOURCES = $(USER_SOURCES_PATH)/libc/syscalls.cpp
		
LIBC = $(USER_BUILD_PATH)/libc.a
LINKER_SCRIPT = -T $(USER_SOURCES_PATH)/linker.ld
INCLUDES = -I$(USER_SOURCES_PATH)/libc/includes
LIBC_LINK = -L$(USER_BUILD_PATH) -lc
LIBC_PATH = $(USER_SOURCES_PATH)/libc

all: $(LIBC) $(USER_SHELL_BINARY)

$(LIBC):
	cd $(LIBC_PATH); make

$(USER_SHELL_BINARY): $(USER_SHELL_OBJECT) $(USER_ENTRY_OBJECT)
	${TARGET}-gcc $(LINKER_SCRIPT) $(FLAGS) $(INCLUDES) $^ $(LIBC_LINK) \
	-fno-rtti -o $@

$(USER_SHELL_OBJECT): $(USER_SHELL_SOURCE)
	${TARGET}-gcc $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(USER_ENTRY_OBJECT): $(USER_ENTRY_SOURCE)
	nasm -f elf64 -g $^ -o $@

.PHONY: clean

clean:
	rm -rf $(USER_BINARY_PATH) $(USER_BUILD_PATH)
	cd $(LIBC_PATH); make clean; cd ../
