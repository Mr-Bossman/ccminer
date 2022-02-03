#!/usr/bin/env bash

# Simple script to create the Makefile and build
git submodule update --init

make distclean || echo clean

rm -f Makefile.in
rm -f config.status

aclocal && autoheader && automake --add-missing --gnu --copy && autoconf

extracflags="-ffast-math -Ofast -funroll-loops -finline-functions -falign-functions=16 -falign-jumps=16 -falign-labels=16 -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -fvariable-expansion-in-unroller -fomit-frame-pointer -fpic -pthread -flto"

export PKG_CONFIG_PATH="/usr/local/opt/openssl/lib/pkgconfig"
export CXXFLAGS="$extracflags"
export CPPFLAGS="$extracflags -I/usr/local/opt/openssl/include"
export CFLAGS="$extracflags"
export LDFLAGS="-v -flto -L/usr/local/opt/openssl/lib"

PATH="$PATH:/usr/local/cuda/bin/" ./configure CXX=clang++ CC=clang --with-no-cuda

if [[ "$(uname -s)" == *"Linux"* ]]; then
make -j $(nproc)
else
make
fi
