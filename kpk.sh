#!/bin/bash

export PREFIX="$HOME/opt/cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"
export KHH_HOME="/home/voidwalker/git-repos/KhhraspokOS"

if [[ $1 == "build" ]]; then
    mkdir build/ bin/
    make

elif [[ $1 == "clean" ]]; then
    make clean
    rm -rf build/ bin/

elif [[ $1 == "run" ]]; then
    qemu-system-x86_64  -smp cores=4,threads=2 -hda KhhraspokOS.bin

elif [[ $1 == "debug" ]]; then
    qemu-system-x86_64  -smp cores=4,threads=2 -hda KhhraspokOS.bin -s -S

elif [[ $1 == "help" ]]; then
    echo "./kpk.sh <option>"
    echo "Options:"
    echo "build         Build the project - obtain the final binary: Khhraspok.bin"
    echo "clean         Clean everything - fresh repo"
    echo "run           Run the Operating System!"
    echo "debug         Start qemu remote debugging session on port 1234"
    echo "help          Show this list"

else
    echo "${1}: option unavailable"
    echo "Run \"${0} help\" to get a list of available options"
fi

