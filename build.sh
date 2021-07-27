#!/bin/bash

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
export KHH_HOME="/home/voidwalker/git-repos/KhhraspokOS"

if [[ $1 == "build" ]]; then
    make
fi

if [[ $1 == "clean" ]]; then
    make clean
fi

