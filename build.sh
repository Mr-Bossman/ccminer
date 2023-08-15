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
./configure --host=arm64-apple-darwin --target=arm64-apple-darwin CC="xcrun -sdk iphoneos gcc -arch arm64" CXX="xcrun -sdk iphoneos g++ -arch arm64 -isysroot ${SYSROOT}" LDFLAGS="-L$test -isysroot ${SYSROOT}" CPPFLAGS="-I$test -miphoneos-version-min=11.0 -Wno-deprecated-declarations" CCFLAGS="-I$test -isysroot ${SYSROOT} -miphoneos-version-min=11.0 -Wno-deprecated-declarations"

make CPPFLAG="-O3"

rm minerd.a
ar cru minerd.a `find . -name "*.o"`
strip minerd.a
