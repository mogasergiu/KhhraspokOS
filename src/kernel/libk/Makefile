KERNEL_SOURCES_PATH = ${KHH_HOME}/src/kernel
BUILD_PATH = ${KHH_HOME}/build/kernel
LIBK_PATH = $(KERNEL_SOURCES_PATH)/libk
LIBK = $(BUILD_PATH)/libk.a
LIBK_STRING_OBJECT = $(BUILD_PATH)/kstring.o
LIBK_STRING_SOURCE = $(LIBK_PATH)/kstring.cpp
LIBK_DS_OBJECT = $(BUILD_PATH)/kdsa.o
LIBK_DS_SOURCE = $(LIBK_PATH)/kdsa.cpp
LIBK_STDIO_OBJECT = $(BUILD_PATH)/kstdio.o
LIBK_STDIO_SOURCE = $(LIBK_PATH)/kstdio.cpp
LIBK_STDLIB_OBJECT = $(BUILD_PATH)/kstdlib.o
LIBK_STDLIB_SOURCE = $(LIBK_PATH)/kstdlib.cpp
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
