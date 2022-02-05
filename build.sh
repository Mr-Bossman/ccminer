#!/usr/bin/env bash

# if ran in another dir
cd "$(dirname "${BASH_SOURCE[0]}")"
# Simple script to create the Makefile and build
if command -v pkg &> /dev/null
then
	pkg update -y
	pkg install -y git automake build-essential clang binutils
fi
if command -v apt &> /dev/null
then
	sudo apt update -y && sudo apt install -y git automake build-essential clang binutils
fi
if [[ "$(uname -s)" == *"Darwin"* ]]; then
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
xcode-select --install
brew install automake binutils llvm libtool
fi

git config --global pull.rebase true
git pull

git submodule update --init --recursive

rm -f Makefile.in
rm -f config.status
aclocal && autoheader && automake --add-missing --copy && autoconf

make distclean

extracflags="-ffast-math -Ofast -funroll-loops -finline-functions -falign-functions=16 -falign-jumps=16 -falign-labels=16 -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -fvariable-expansion-in-unroller -fomit-frame-pointer -fpic -pthread -flto -Wl,-hugetlbfs-align -mtune=native -mllvm -enable-loop-distribute"
if command -v clang &> /dev/null
then
./configure --with-no-cuda CC=clang CXX=clang++ CXXFLAGS="$extracflags" CFLAGS="$extracflags"
else
./configure --with-no-cuda CXXFLAGS="-O3" CFLAGS="-O3"
fi

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
./configure --with-openssl=`cd ..;pwd`/openssl LDFLAGS="`cd ..;pwd`/openssl/libssl.a `cd ..;pwd`/openssl/libcrypto.a" --with-zlib=`cd ..;pwd`/zlib --without-hyper --without-brotli --without-zstd --without-ca-bundle --without-ca-path --without-ca-fallback --without-libpsl --without-libgsasl --without-librtmp --without-winidn --without-libidn2 --without-nghttp2 --without-ngtcp2 --without-nghttp3 --without-quiche --without-zsh-functions-dir --without-fish-functions-dir --disable-ldap --enable-ipv6 --enable-unix-sockets
cd ..

if [[ "$(uname -s)" == *"Linux"* ]]; then
make -j $(nproc)
else
make
fi
