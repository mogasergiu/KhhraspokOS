KERNEL_SOURCES_PATH = ${KHH_HOME}/src/kernel
BUILD_PATH = ${KHH_HOME}/build
LIBK_PATH = $(KERNEL_SOURCES_PATH)/libk
LIBK = $(BUILD_PATH)/libk.a
LIBK_STRING_OBJECT = $(BUILD_PATH)/string.o
LIBK_STRING_SOURCE = $(LIBK_PATH)/string.cpp
LIBK_DS_OBJECT = $(BUILD_PATH)/ds.o
LIBK_DS_SOURCE = $(LIBK_PATH)/ds.cpp
LIBK_STDIO_OBJECT = $(BUILD_PATH)/stdio.o
LIBK_STDIO_SOURCE = $(LIBK_PATH)/stdio.cpp
LIBK_STDLIB_OBJECT = $(BUILD_PATH)/stdlib.o
LIBK_STDLIB_SOURCE = $(LIBK_PATH)/stdlib.cpp
LIBK_OBJECTS = $(LIBK_STRING_OBJECT) \
	       $(LIBK_DS_OBJECT) \
	       $(LIBK_STDIO_OBJECT) \
	       $(LIBK_STDLIB_OBJECT)
INCLUDES = -I$(KERNEL_SOURCES_PATH)/includes -I$(LIBK_PATH)/includes
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
        -falign-loops -fstrength-reduce -fomit-frame-pointer \
        -finline-functions -Wno-unused-function -fno-builtin -Werror -Wall \
        -Wextra -Wno-unused-label -Wno-unused-parameter -nostdlib \
        -nostartfiles -nodefaultlibs -O0 -std=c++20 -m64 -mno-mmx \
	-mno-sse -mno-sse2

all: $(LIBK)

$(LIBK): $(LIBK_OBJECTS)
	ar cr $@ $^

$(LIBK_STRING_OBJECT): $(LIBK_STRING_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(LIBK_DS_OBJECT): $(LIBK_DS_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(LIBK_STDIO_OBJECT): $(LIBK_STDIO_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

$(LIBK_STDLIB_OBJECT): $(LIBK_STDLIB_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

.PHONY: clean

clean:
	rm -f $(LIBK) $(LIBK_OBJECTS)
