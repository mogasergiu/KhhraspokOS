#!/bin/bash

export PREFIX="${HOME}/opt/cross"
export TARGET=x86_64-elf
export PATH="${PREFIX}/bin:${PATH}"
export KHH_HOME=${PWD}

function mountFS() {
    sudo mkdir /mnt/kpk
    sudo mount ${KHH_HOME}/KhhraspokOS.bin /mnt/kpk -t vfat
    cd /mnt/kpk
    sudo mkdir bin
    sudo cp ${KHH_HOME}/bin/user/* bin/
    cd ..
    sudo umount kpk
    cd ${KHH_HOME}
}

function makeToolchain() {
    mkdir -p ${HOME}/opt/cross

    mkdir /tmp/src

    cd /tmp/src 
    wget https://ftp.gnu.org/gnu/binutils/binutils-2.37.tar.xz
    tar xvf binutils-2.37.tar.xz
    mkdir build-binutils
    cd build-binutils
    ../binutils-2.37/configure  --target=${TARGET} \
                                --prefix="${PREFIX}" \
                                --with-sysroot \
                                --disable-nls \
                                --disable-werror
    make
    make install

    cd /tmp/src
    wget https://ftp.gnu.org/gnu/gcc/gcc-11.2.0/gcc-11.2.0.tar.xz
    tar xvf gcc-11.2.0.tar.xz
    mkdir build-gcc
    cd build-gcc
    ../gcc-11.2.0/configure --target=${TARGET} \
                            --prefix="${PREFIX}" \
                            --disable-nls \
                            --enable-languages=c,c++ \
                            --without-headers
    make all-gcc
    make all-target-libgcc
    make install-gcc
    make install-target-libgcc
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

elif [[ $1 == "toolchain" ]]; then
    makeToolchain

elif [[ $1 == "help" ]]; then
    echo "./kpk.sh <option>"
    echo "Options:"
    echo "build         Build the project - obtain the final binary: Khhraspok.bin"
    echo "clean         Clean everything - fresh repo"
    echo "run           Run the Operating System!"
    echo "dump          Dump Interrupts"
    echo "toolchain     Install the toolchain KhhraspokOS was built with (takes a while)"
    echo "debug         Start qemu remote debugging session on port 1234"
    echo "help          Show this list"

else
    echo "${1}: option unavailable"
    echo "Run \"${0} help\" to get a list of available options"
fi

