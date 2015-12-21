#!/usr/bin/env bash

# This should be run inside vagrant, see build.bat

export PREFIX="$HOME/opt/cross/"
export PATH="$PREFIX/bin:$PATH"

cd /vagrant

mkdir -p bin
i686-elf-as src/boot.s -o bin/boot.o

