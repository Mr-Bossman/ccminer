#!/bin/bash

# Simple script to create the Makefile and build

# export PATH="$PATH:/usr/local/cuda/bin/"

make distclean || echo clean

rm -f Makefile.in
rm -f config.status
./autogen.sh || echo done
test=`pwd`
# CFLAGS="-O2" ./configure
./configure --host=arm64-apple-darwin --target=arm64-apple-darwin CC="xcrun -sdk iphoneos gcc -arch arm64 -static-libstdc++" CXX="xcrun -sdk iphoneos g++ -arch arm64 -static-libstdc++" LDFLAGS="-L$test"

make

ar cru minerd.a `find . -name "*.o"`
