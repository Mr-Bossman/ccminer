#!/usr/bin/env bash

# Simple script to create the Makefile and build
git submodule update --init

make distclean || echo clean

rm -f Makefile.in
rm -f config.status

aclocal && autoheader && automake --add-missing --gnu --copy && autoconf

extracflags="-ffast-math -Ofast -funroll-loops -finline-functions -falign-functions=16 -falign-jumps=16 -falign-labels=16 -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -Wl,-hugetlbfs-align  -fvariable-expansion-in-unroller -fomit-frame-pointer -fpic -pthread -flto"

export CXXFLAGS="$extracflags"
export CPPFLAGS="$extracflags"
export CFLAGS="$extracflags"
export LDFLAGS="-v -flto  -Wl,-hugetlbfs-align"

PATH="$PATH:/usr/local/cuda/bin/:/usr/local/opt/openssl/" ./configure CXX=clang++ CPP=clang-cpp CC=clang --with-no-cuda

make -j $(nproc)
