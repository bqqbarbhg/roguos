#!/usr/bin/env bash

# Require all variables to be set and exit on first error.
set -u
set -e

LOG='/vagrant/provisioning/provisioning.log'

# Clear the log
> $LOG

# A quiet unattended installation
export DEBIAN_FRONTEND=noninteractive

export PREFIX="$HOME/opt/cross/"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir -p build-cross &>> $LOG
cd build-cross &>> $LOG

GCC_VER=gcc-5.3.0
BINUTILS_VER=binutils-2.25.1

cd $HOME

if [ -d $GCC_VER -a -d $BINUTILS_VER ]; then
    echo "Found sources for: $GCC_VER, $BINUTILS_VER" 
else
    echo "Downloading sources: $GCC_VER, $BINUTILS_VER"

    wget -q ftp://ftp.gnu.org/gnu/gcc/${GCC_VER}/${GCC_VER}.tar.bz2 \
            ftp://ftp.gnu.org/gnu/binutils/${BINUTILS_VER}.tar.bz2 &>> $LOG
    tar jxf ${GCC_VER}.tar.bz2 &>> $LOG
    tar jxf ${BINUTILS_VER}.tar.bz2 &>> $LOG

    rm -f ${GCC_VER}.tar.bz2 &>> $LOG
    rm -f ${BINUTILS_VER}.tar.bz2 &>> $LOG
fi

echo 'Installing prerequisites'

sudo apt-get -qy install g++ libmpfr-dev libgmp3-dev libmpc-dev \
                    flex bison texinfo &>> $LOG

sudo apt-get -qy upgrade xorriso

# Vagrant runs out of memory when building GCC, so use a swapfile
SWAP=/tmp/swap
if [ -f $SWAP ]; then
    echo 'Found swapfile'
else
    echo 'Creating swapfile'

    dd if=/dev/zero of=$SWAP bs=1M count=500 &>> $LOG
    mkswap $SWAP &>> $LOG
    sudo swapon $SWAP &>> $LOG
fi

# All bintools most probably exist if ld exists
if [ -f $PREFIX/bin/$TARGET-ld ]; then
    echo "Found binaries for $BINUTILS_VER"
else
    echo "Configuring $BINUTILS_VER"

    cd $BINUTILS_VER &>> $LOG
    mkdir -p build &>> $LOG
    cd build &>> $LOG

    sudo ../configure \
        --target=$TARGET \
        --prefix="$PREFIX" \
        --with-sysroot \
        --disable-nls \
        --disable-werror &>> $LOG

    echo "Building $BINUTILS_VER"

    sudo make &>> $LOG
    sudo make install &>> $LOG

    cd ../.. &>> $LOG
fi

# All gcc binaries most probably exist if gcc exists
if [ -f $PREFIX/bin/$TARGET-gcc ]; then
    echo "Found binaries for $GCC_VER"
else
    echo "Cofiguring $GCC_VER"

    cd $GCC_VER &>> $LOG
    mkdir -p build &>> $LOG
    cd build &>> $LOG

    sudo ../configure \
        --target=$TARGET \
        --prefix="$PREFIX" \
        --disable-nls \
        --enable-languages=c,c++ \
        --without-headers &>> $LOG

    echo "Building $GCC_VER"

    sudo make all-gcc &>> $LOG || true
    sudo make all-target-libgcc &>> $LOG || true
    sudo make install-gcc &>> $LOG || true
    sudo make install-target-libgcc &>> $LOG || true

    cd ../.. &>> $LOG
fi

sudo swapoff $SWAP &>> $LOG
rm -f $SWAP &>> $LOG

echo 'Done!'

