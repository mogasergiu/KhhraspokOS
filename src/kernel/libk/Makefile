KERNEL_SOURCES_PATH = ${KHH_HOME}/src/kernel
BUILD_PATH = ${KHH_HOME}/build
LIBK_PATH = $(KERNEL_SOURCES_PATH)/libk
LIBK = $(BUILD_PATH)/libk.a
LIBK_STRING_OBJECT = $(LIBK_PATH)/string.o
LIBK_STRING_SOURCE = $(LIBK_PATH)/string.cpp
LIBK_OBJECTS = $(LIBK_STRING_OBJECT)
INCLUDES = -I$(KERNEL_SOURCES_PATH)/includes -I$(LIBK_PATH)/includes
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
        -falign-loops -fstrength-reduce -fomit-frame-pointer \
        -finline-functions -Wno-unused-function -fno-builtin -Werror -Wall \
        -Wextra -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib \
        -nostartfiles -nodefaultlibs -O0 -Iinc -std=c++20

all: $(LIBK)

$(LIBK): $(LIBK_OBJECTS)
	ar cr $@ $^

$(LIBK_STRING_OBJECT): $(LIBK_STRING_SOURCE)
	${TARGET}-g++ $(FLAGS) $(INCLUDES) -c $^ -fno-rtti -o $@

.PHONY: clean

clean:
	rm -f $(LIBK) $(LIBK_OBJECTS)
