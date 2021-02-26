#!/bin/bash

# Simple script to create the Makefile and build

# export PATH="$PATH:/usr/local/cuda/bin/"

make distclean || echo clean

rm -f Makefile.in
rm -f config.status
./autogen.sh || echo done
test=`pwd`
readonly XCODE_DEV="$(xcode-select -p)"
SYSROOT=${XCODE_DEV}/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk 
# CFLAGS="-O2" ./configure
./configure --host=arm64-apple-darwin --target=arm64-apple-darwin CC="xcrun -sdk iphoneos gcc -arch arm64" CXX="xcrun -sdk iphoneos g++ -arch arm64 -isysroot ${SYSROOT}" LDFLAGS="-L$test -isysroot ${SYSROOT} -framework Security" CPPFLAGS="-I$test -miphoneos-version-min=8.0 -framework Security" CCFLAGS="-I$test -isysroot ${SYSROOT} -framework Security -miphoneos-version-min=8.0"

make

ar cru minerd.a `find . -name "*.o"`
