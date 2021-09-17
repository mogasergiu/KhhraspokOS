FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
        -falign-loops -fomit-frame-pointer \
        -finline-functions -Wno-unused-function -fno-builtin -Werror -Wall \
        -Wextra -Wno-unused-label -Wno-unused-parameter -nostdlib \
        -nostartfiles -nodefaultlibs -O0 -std=c++20 -mno-red-zone \
	-mgeneral-regs-only -m64 -mno-mmx -mno-sse -mno-sse2 -fstrength-reduce
USER_SOURCES_PATH = ${KHH_HOME}/src/user
USER_BUILD_PATH = ${KHH_HOME}/build/user
USER_BINARY_PATH = ${KHH_HOME}/bin/user
USER_TEST_SOURCE = $(USER_SOURCES_PATH)/programs/test.asm
USER_TEST_OBJECT = $(USER_BUILD_PATH)/test.o
USER_TEST_BINARY = $(USER_BINARY_PATH)/test
LINKER_SCRIPT = -T $(USER_SOURCES_PATH)/linker.ld
LIBC_LINK = -L$(USER_BUILD_PATH) -lk
LINKER_FLAGS = -g -relocatable
INCLUDES = -I$(USER_SOURCES_PATH)/includes

all: $(USER_TEST_BINARY)

$(USER_TEST_BINARY): $(USER_TEST_OBJECT)
	${TARGET}-g++ $(LINKER_SCRIPT) $(FLAGS) $(INCLUDES) $^ -fno-rtti -o $@

$(USER_TEST_OBJECT): $(USER_TEST_SOURCE)
	nasm -f elf64 -g $^ -o $@

.PHONY: clean

clean:
	rm -rf $(USER_BINARY_PATH)/ $(USER_BUILD_PATH)/
