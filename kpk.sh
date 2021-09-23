#!/bin/bash

export PREFIX="$HOME/opt/cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"
export KHH_HOME="/home/voidwalker/git-repos/KhhraspokOS"

function mountFS() {
    sudo mkdir /mnt/kpk
    sudo mount ${KHH_HOME}/KhhraspokOS.bin /mnt/kpk -t vfat
    cd /mnt/kpk
    for i in $(seq 1 1000); do echo "Hello world1"; done | sudo tee 1.txt 1>/dev/null
    sudo mkdir dir
    for i in $(seq 1 1000); do echo "Hello dir"; done | sudo tee dir/2.txt 1>/dev/null
    sudo cp ${KHH_HOME}/bin/user/* dir/
    cd ..
    sudo umount kpk
    cd ${KHH_HOME}
}

if [[ $1 == "build" ]]; then
    mkdir -p build/kernel build/user bin/kernel bin/user bin/boot
    make
    mountFS

elif [[ $1 == "clean" ]]; then
    make clean
    rm -rf build/ bin/ 
    sudo rm -rf /mnt/kpk/

elif [[ $1 == "run" ]]; then
    qemu-system-x86_64  -smp 4 -hda KhhraspokOS.bin -vga std

elif [[ $1 == "dump" ]]; then
    qemu-system-x86_64  -smp 4 -hda KhhraspokOS.bin -d int --no-reboot

elif [[ $1 == "debug" ]]; then
    qemu-system-x86_64  -smp 4 -hda KhhraspokOS.bin -s -S

elif [[ $1 == "help" ]]; then
    echo "./kpk.sh <option>"
    echo "Options:"
    echo "build         Build the project - obtain the final binary: Khhraspok.bin"
    echo "clean         Clean everything - fresh repo"
    echo "run           Run the Operating System!"
    echo "dump          See CPU crash dump"
    echo "debug         Start qemu remote debugging session on port 1234"
    echo "help          Show this list"

else
    echo "${1}: option unavailable"
    echo "Run \"${0} help\" to get a list of available options"
fi

