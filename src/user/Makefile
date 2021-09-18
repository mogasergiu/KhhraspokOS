FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
        -falign-loops -fomit-frame-pointer \
        -finline-functions -Wno-unused-function -fno-builtin -Werror -Wall \
        -Wextra -Wno-unused-label -Wno-unused-parameter -nostdlib \
        -nostartfiles -nodefaultlibs -O0 -std=c++20 -mno-red-zone \
	-mgeneral-regs-only -m64 -mno-mmx -mno-sse -mno-sse2 -fstrength-reduce
USER_SOURCES_PATH = ${KHH_HOME}/src/user
USER_BUILD_PATH = ${KHH_HOME}/build/user
USER_BINARY_PATH = ${KHH_HOME}/bin/user
USER_TEST_SOURCE = $(USER_SOURCES_PATH)/programs/1.asm
USER_TEST_OBJECT = $(USER_BUILD_PATH)/1.o
USER_TEST_BINARY = $(USER_BINARY_PATH)/1
USER_TEST_SOURCE2 = $(USER_SOURCES_PATH)/programs/2.asm
USER_TEST_OBJECT2 = $(USER_BUILD_PATH)/2.o
USER_TEST_BINARY2 = $(USER_BINARY_PATH)/2
LIBC_SOURCES = $(USER_SOURCES_PATH)/libc/syscalls.cpp
		
LIBC = $(USER_BUILD_PATH)/libc.a
LINKER_SCRIPT = -T $(USER_SOURCES_PATH)/linker.ld
LINKER_FLAGS = -g -relocatable
INCLUDES = -I$(USER_SOURCES_PATH)/libc/includes
LIBC_LINK = -L$(USER_BUILD_PATH) -lc
LIBC_PATH = $(USER_SOURCES_PATH)/libc

all: $(USER_TEST_BINARY) $(LIBC_OBJECT) $(USER_TEST_BINARY2)

$(LIBC):
	cd $(LIBC_PATH); make

$(USER_TEST_BINARY): $(USER_TEST_OBJECT) $(LIBC)
	${TARGET}-g++ $(LINKER_SCRIPT) $(FLAGS) $(INCLUDES) $^ $(LIBC_LINK) \
	-fno-rtti -o $@

$(USER_TEST_OBJECT): $(USER_TEST_SOURCE)
	nasm -f elf64 -g $^ -o $@

$(USER_TEST_BINARY2): $(USER_TEST_OBJECT2) $(LIBC)
	${TARGET}-g++ $(LINKER_SCRIPT) $(FLAGS) $(INCLUDES) $^ $(LIBC_LINK) \
	-fno-rtti -o $@

$(USER_TEST_OBJECT2): $(USER_TEST_SOURCE2)
	nasm -f elf64 -g $^ -o $@

.PHONY: clean

clean:
	rm -rf $(USER_BINARY_PATH) $(USER_BUILD_PATH)
	rm -rf $(USER_BINARY_PATH2) $(USER_BUILD_PATH2)
	cd $(LIBC_PATH); make clean; cd ../
