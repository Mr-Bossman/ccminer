#ifndef PORT_H
#define PORT_H
#define ARM
#ifdef ARM
#include "sse2neon/sse2neon.h"
#define NCPUID
#else
//#include <x86intrin.h>
#include <immintrin.h>
#ifdef _WIN32
#include <intrin.h>
#else
#include <cpuid.h>
#endif // !WIN32
#endif


typedef __m128i u128;

#ifdef __APPLE__
#include <sys/types.h>
#endif// APPLE

#include "verus_clhash.h"


#ifdef __cplusplus
#include "uint256.h"
extern "C"
{
#include "haraka.h"
}
#else
#include "haraka.h"
#endif
#endif /* PORT_H */