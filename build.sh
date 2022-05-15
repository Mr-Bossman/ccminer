#!/usr/bin/env bash
if [ ! $THREADS ]; then
	export THREADS=1
fi
# if ran in another dir
cd "$(dirname "${BASH_SOURCE[0]}")"
# Simple script to create the Makefile and build
if command -v pkg &> /dev/null
then
	pkg update -y
	pkg install -y git automake build-essential clang binutils libtool
fi
if command -v apt &> /dev/null
then
	sudo apt update -y && sudo apt install -y git automake build-essential clang binutils libtool
fi
if [[ "$(uname -s)" == *"Darwin"* ]]; then
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
xcode-select --install
brew install automake binutils llvm libtool
ln -s /usr/local/bin/glibtoolize /usr/local/bin/libtoolize
export CFLAGS="-isysroot $(xcrun -sdk macosx --show-sdk-path)"
export LDFLAGS="-isysroot $(xcrun -sdk macosx --show-sdk-path) -framework SystemConfiguration -framework CoreFoundation"
fi

git config --global pull.rebase true
git pull

git submodule update --init --recursive

rm -f Makefile.in
rm -f config.status
aclocal && autoheader && automake --add-missing --copy && autoconf

make distclean

extracflags="-ffast-math -Ofast -funroll-loops -finline-functions -falign-functions=16 -falign-jumps=16 -falign-labels=16 -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -fvariable-expansion-in-unroller -fomit-frame-pointer -fpic -pthread -flto -mtune=native -mllvm -enable-loop-distribute"
if command -v clang &> /dev/null
then
./configure --with-no-cuda CC=clang CXX=clang++ CXXFLAGS="$extracflags" CFLAGS="$extracflags"
else
./configure --with-no-cuda CXXFLAGS="-O3" CFLAGS="-O3"
fi

cd zlib
./configure
cd ../openssl
./Configure  --prefix=`cd ..;pwd`/ssl  --with-zlib-include=`cd ..;pwd`/zlib --with-zlib-lib=`cd ..;pwd`/zlib
if [[ "$(uname -s)" == *"Linux"* ]]; then
make -j $(nproc)
else
make -j $THREADS
fi
make install
cd ../jansson
libtoolize; aclocal && autoheader && automake --add-missing --copy && autoconf
./configure
cd ../curl
./buildconf
./configure --with-openssl=`cd ..;pwd`/ssl --with-zlib=`cd ..;pwd`/zlib --without-hyper --without-brotli --without-zstd --without-ca-bundle --without-ca-path --without-ca-fallback --without-libpsl --without-libgsasl --without-librtmp --without-winidn --without-libidn2 --without-nghttp2 --without-ngtcp2 --without-nghttp3 --without-quiche --without-zsh-functions-dir --without-fish-functions-dir --disable-ldap --enable-ipv6 --enable-unix-sockets --disable-shared --enable-static
cd ..

if [[ "$(uname -s)" == *"Linux"* ]]; then
make -j $(nproc)
else
make -j $THREADS
fi
strip ccminer
