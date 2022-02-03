#!/usr/bin/env bash

# Simple script to create the Makefile and build
git submodule update --init --recursive

make distclean || echo clean

rm -f Makefile.in
rm -f config.status
aclocal && autoheader && automake --add-missing --copy && autoconf

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
aclocal && autoheader && automake --add-missing --copy && autoconf
./configure
cd ../curl
./buildconf
./configure  --enable-shared=no LDFLAGS="-L`cd ..;pwd`/openssl/" --with-zlib=`cd ..;pwd`/zlib --with-openssl=`cd ..;pwd`/openssl
cd ..

extracflags="-O3 -ffast-math -Ofast -funroll-loops -finline-functions -falign-functions=16 -falign-jumps=16 -falign-labels=16 -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -fvariable-expansion-in-unroller -fomit-frame-pointer -fpic -pthread -flto"
if command -v clang &> /dev/null
then
CXX=clang++
CC=clang
fi
PATH="$PATH:/usr/local/cuda/bin/" ./configure --with-no-cuda
if [[ "$(uname -s)" == *"Linux"* ]]; then
make -j $(nproc)
else
make
fi
