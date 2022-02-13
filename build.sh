#!/usr/bin/env bash

# if ran in another dir
cd "$(dirname "${BASH_SOURCE[0]}")"
#exit on any error
set -e
# Simple script to create the Makefile and build
if command -v pkg &> /dev/null
then
	pkg update -y
	pkg install -y git automake build-essential clang binutils
fi
if command -v apt &> /dev/null
then
	if [ `dpkg --list | grep -w " git \| automake \| build-essential \| clang \| binutils "  | wc -l` -lt "5" ]
	then
	sudo apt update -y && sudo apt install -y git automake build-essential clang binutils
	fi
fi
if [[ "$(uname -s)" == *"Darwin"* ]]; then
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
xcode-select --install
brew install automake binutils llvm libtool
fi

git config --global pull.rebase true
git pull && true

git submodule update --init --recursive

aclocal && autoheader && automake --add-missing --copy && autoconf

echo cleaning build
make distclean &>/dev/null && true

extracflags="-O3 -ffast-math -Ofast -funroll-loops -finline-functions -falign-functions=16 -falign-jumps=16 -falign-labels=16 -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -fvariable-expansion-in-unroller -fomit-frame-pointer -fpic -pthread -flto"
if command -v clang &> /dev/null
then
CXX=clang++
CC=clang
fi
./configure --with-no-cuda

cd zlib
./configure
cd ../openssl
./Configure
if [[ "$(uname -s)" == *"Linux"* ]]; then
make -j $(nproc)
else
make
fi
cd ../jansson
libtoolize && aclocal && autoheader && automake --add-missing --copy && autoconf
./configure
cd ../curl
./buildconf
./configure --with-openssl=`cd ..;pwd`/openssl LDFLAGS="`cd ..;pwd`/openssl/libssl.a `cd ..;pwd`/openssl/libcrypto.a" --with-zlib=`cd ..;pwd`/zlib --without-hyper --without-brotli --without-zstd --without-ca-bundle --without-ca-path --without-ca-fallback --without-libpsl --without-libgsasl --without-librtmp --without-winidn --without-libidn2 --without-nghttp2 --without-ngtcp2 --without-nghttp3 --without-quiche --without-zsh-functions-dir --without-fish-functions-dir
cd ..

if [[ "$(uname -s)" == *"Linux"* ]]; then
make -j $(nproc)
else
make
fi
