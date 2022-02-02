# allow to use Host cuda functions in C/C++
DEF_INCLUDES = @CUDA_INCLUDES@

if WANT_JANSSON
JANSSON_INCLUDES= -I$(top_srcdir)/compat/jansson
else
JANSSON_INCLUDES=
endif

EXTRA_DIST	= autogen.sh README.md README.txt LICENSE.txt \
		  ccminer.sln ccminer.vcxproj ccminer.vcxproj.filters \
		  compat/gettimeofday.c compat/getopt/getopt_long.c

SUBDIRS = compat

bin_PROGRAMS = ccminer

ccminer_SOURCES	= elist.h miner.h compat.h \
		  compat/inttypes.h compat/stdbool.h compat/unistd.h bignum.cpp bignum.hpp \
		  compat/sys/time.h compat/getopt/getopt.h \
		  crc32.c \
		  ccminer.cpp pools.cpp util.cpp \
		  api.cpp hashlog.cpp stats.cpp sysinfos.cpp \
		  equi/equi-stratum.cpp \
		  verus/verusscan.cpp verus/haraka.c verus/verus_clhash.cpp verus/verus-stratum.cpp

if HAVE_NVML
nvml_defs = -DUSE_WRAPNVML
nvml_libs = -ldl
endif

if HAVE_WINDOWS
ccminer_SOURCES += compat/winansi.c
endif

ccminer_LDFLAGS  = $(PTHREAD_FLAGS) @CUDA_LDFLAGS@
ccminer_LDADD    = @JANSSON_LIBS@ @PTHREAD_LIBS@ @WS2_LIBS@ @CUDA_LIBS@ @OPENMP_CFLAGS@ @LIBS@ $(nvml_libs)
ccminer_CPPFLAGS = @OPENMP_CFLAGS@  @ARCH_FLAGS@ $(CPPFLAGS) $(PTHREAD_FLAGS) -fno-strict-aliasing $(JANSSON_INCLUDES) $(DEF_INCLUDES) $(nvml_defs)
ccminer_CXXFLAGS = -std=c++11  @ARCH_FLAGS@
ccminer_CFLAGS = @ARCH_FLAGS@

if ARCH_ARM64
ccminer_CPPFLAGS += -DARM64
endif

if HAVE_OSX
ccminer_CXXFLAGS += -std=c++11
ccminer_CPPFLAGS += -I/usr/local/llvm/lib/clang/4.0.0/include
ccminer_LDFLAGS += -L/usr/local/llvm/lib
ccminer_LDADD += -lcurl
else
ccminer_CPPFLAGS += @LIBCURL_CPPFLAGS@
ccminer_LDADD += @LIBCURL@
endif

if !NO_CUDA
if ARCH_ARM64
ccminer_LDFLAGS += -L@CUDA_LOCAL_PATH@/targets/aarch64-linux/lib
else
  # equihash
  ccminer_SOURCES += equi/equi.cpp equi/blake2/blake2bx.cpp equi/equihash.cpp equi/cuda_equi.cu

  # scrypt
  ccminer_SOURCES += scrypt.cpp scrypt-jane.cpp \
    scrypt/blake.cu scrypt/keccak.cu scrypt/sha256.cu \
    scrypt/salsa_kernel.cu scrypt/test_kernel.cu \
    scrypt/fermi_kernel.cu scrypt/kepler_kernel.cu \
    scrypt/nv_kernel.cu scrypt/nv_kernel2.cu scrypt/titan_kernel.cu
endif

#ccminer_CPPFLAGS += -DUSE_LIBSODIUM
#ccminer_LDFLAGS += -Lequi/lib
#ccminer_LDADD += -lsodium
ccminer_LDADD += -lcuda

nvcc_ARCH :=
#nvcc_ARCH += -gencode=arch=compute_75,code=\"sm_75,compute_75\" # CUDA 10 req.
#nvcc_ARCH += -gencode=arch=compute_72,code=\"sm_72,compute_72\" # CUDA 10 req.
#nvcc_ARCH += -gencode=arch=compute_70,code=\"sm_70,compute_70\" # CUDA 9.1
#nvcc_ARCH += -gencode=arch=compute_61,code=\"sm_61,compute_61\" # CUDA 8
nvcc_ARCH += -gencode=arch=compute_52,code=\"sm_52,compute_52\"
#nvcc_ARCH += -gencode=arch=compute_50,code=\"sm_50,compute_50\"
#nvcc_ARCH += -gencode=arch=compute_35,code=\"sm_35,compute_35\"
#nvcc_ARCH += -gencode=arch=compute_30,code=\"sm_30,compute_30\"

nvcc_FLAGS = $(nvcc_ARCH) @CUDA_INCLUDES@ -I. @CUDA_CFLAGS@
nvcc_FLAGS += $(JANSSON_INCLUDES) --ptxas-options="-v"

# we're now targeting all major compute architectures within one binary.
.cu.o:
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=128 -o $@ -c $<

Algo256/blake256.o: Algo256/blake256.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=64 -o $@ -c $<

Algo256/cuda_bmw.o: Algo256/cuda_bmw.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=76 -o $@ -c $<

crypto/cryptonight-core.o: crypto/cryptonight-core.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=64 -o $@ -c $<

crypto/cryptonight-extra.o: crypto/cryptonight-extra.cu
	$(NVCC) $(nvcc_FLAGS) -o $@ -c $<

heavy/cuda_hefty1.o: heavy/cuda_hefty1.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=80 -o $@ -c $<

qubit/qubit_luffa512.o: qubit/qubit_luffa512.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=80 -o $@ -c $<

# Luffa and Echo are faster with 80 registers than 128
x11/cuda_x11_luffa512.o: x11/cuda_x11_luffa512.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=80 -o $@ -c $<

x11/cuda_x11_luffa512_Cubehash.o: x11/cuda_x11_luffa512_Cubehash.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=76 -o $@ -c $<

x11/cuda_x11_simd512.o: x11/cuda_x11_simd512.cu
	$(NVCC) $(nvcc_FLAGS) -Xcompiler -Wno-unused-variable -o $@ -c $<

x13/cuda_x13_hamsi512.o: x13/cuda_x13_hamsi512.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=72 -o $@ -c $<

x17/cuda_x17_sha512.o: x17/cuda_x17_sha512.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=80 -o $@ -c $<

quark/cuda_quark_blake512.o: quark/cuda_quark_blake512.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=80 -o $@ -c $<

quark/cuda_jh512.o: quark/cuda_jh512.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=80 -o $@ -c $<

quark/cuda_quark_keccak512.o: quark/cuda_quark_keccak512.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=88 -o $@ -c $<

quark/cuda_quark_compactionTest.o: quark/cuda_quark_compactionTest.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=80 -o $@ -c $<

JHA/cuda_jha_compactionTest.o: JHA/cuda_jha_compactionTest.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=80 -o $@ -c $<

# This object does not use cuda device code but call the different kernels (autotune)
scrypt/salsa_kernel.o: scrypt/salsa_kernel.cu
	$(NVCC) $(JANSSON_INCLUDES) -I. @CUDA_INCLUDES@ @CUDA_CFLAGS@ -gencode=arch=compute_35,code=\"sm_35,compute_35\" -o $@ -c $<

# These kernels are for older devices (SM)

# Originally sm_20
scrypt/test_kernel.o: scrypt/test_kernel.cu
	$(NVCC) $(JANSSON_INCLUDES) -I. @CUDA_INCLUDES@ @CUDA_CFLAGS@ -gencode=arch=compute_35,code=\"sm_35,compute_35\" -o $@ -c $<

# Originally sm_20
scrypt/fermi_kernel.o: scrypt/fermi_kernel.cu
	$(NVCC) $(JANSSON_INCLUDES) -I. @CUDA_INCLUDES@ @CUDA_CFLAGS@ -gencode=arch=compute_35,code=\"sm_35,compute_35\" -o $@ -c $<

# Originally sm_30
scrypt/kepler_kernel.o: scrypt/kepler_kernel.cu
	$(NVCC) $(JANSSON_INCLUDES) -I. @CUDA_INCLUDES@ @CUDA_CFLAGS@ -gencode=arch=compute_35,code=\"sm_35,compute_35\" -o $@ -c $<

# Originally sm_30
scrypt/nv_kernel.o: scrypt/nv_kernel.cu
	$(NVCC) $(JANSSON_INCLUDES) -I. @CUDA_INCLUDES@ @CUDA_CFLAGS@ -gencode=arch=compute_35,code=\"sm_35,compute_35\" -o $@ -c $<

scrypt/titan_kernel.o: scrypt/titan_kernel.cu
	$(NVCC) $(nvcc_FLAGS) -gencode=arch=compute_35,code=\"sm_35,compute_35\" -o $@ -c $<

skein.o: skein.cu
	$(NVCC) $(nvcc_FLAGS) --maxrregcount=64 -o $@ -c $<

endif
