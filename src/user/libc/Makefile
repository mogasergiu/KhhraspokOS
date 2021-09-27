USER_SOURCES_PATH = ${KHH_HOME}/src/user
BUILD_PATH = ${KHH_HOME}/build/user
LIBC_PATH = $(USER_SOURCES_PATH)/libc
LIBC = $(BUILD_PATH)/libc.a
LIBC_STDIO_OBJECT = $(BUILD_PATH)/stdio.o
LIBC_STDIO_SOURCE = $(LIBC_PATH)/stdio.cpp
LIBC_SYSCALLS_OBJECT = $(BUILD_PATH)/syscalls.o
LIBC_SYSCALLS_SOURCE = $(LIBC_PATH)/syscalls.cpp
LIBC_STDLIB_OBJECT = $(BUILD_PATH)/stdlib.o
LIBC_STDLIB_SOURCE = $(LIBC_PATH)/stdlib.cpp
LIBC_STRING_OBJECT = $(BUILD_PATH)/string.o
LIBC_STRING_SOURCE = $(LIBC_PATH)/string.cpp
LIBC_OBJECTS =  $(LIBC_STDIO_OBJECT) \
		$(LIBC_SYSCALLS_OBJECT) \
		$(LIBC_STDLIB_OBJECT) \
		$(LIBC_STRING_OBJECT)
INCLUDES = -I$(LIBC_PATH)/includes
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
        -falign-loops -fstrength-reduce -fomit-frame-pointer \
        -finline-functions -Wno-unused-function -fno-builtin -Werror -Wall \
        -Wextra -Wno-unused-label -Wno-unused-parameter -nostdlib \
        -nostartfiles -nodefaultlibs -O0 -std=c++20 -m64 -mno-mmx \
	-mno-sse -mno-sse2

all: $(LIBC)

$(LIBC): $(LIBC_OBJECTS)
	ar cr $@ $^

$(LIBC_STDIO_OBJECT): $(LIBC_STDIO_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(LIBC_STDLIB_OBJECT): $(LIBC_STDLIB_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(LIBC_STRING_OBJECT): $(LIBC_STRING_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(LIBC_SYSCALLS_OBJECT): $(LIBC_SYSCALLS_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

.PHONY: clean

clean:
	rm -f $(LIBC) $(LIBC_OBJECTS)
