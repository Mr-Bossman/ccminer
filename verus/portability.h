#ifndef PORT_H
#define PORT_H
#ifdef ARM
#include "sse2neon/sse2neon.h"
#define NCPUID
#else
#include <immintrin.h>
#ifdef _WIN32
#include <intrin.h>
#else
#include <cpuid.h>
#include <x86intrin.h>
#endif // !WIN32
#endif

#ifdef _WIN32

#define posix_memalign(p, a, s) (((*(p)) = (u128*) _aligned_malloc((s), (a))), *(p) ?0 :errno)
#endif

typedef __m128i u128;



#ifdef _MSC_VER
# define __func__ __FUNCTION__
# define __thread __declspec(thread)
# define _ALIGN(x) __declspec(align(x))
#else
# define _ALIGN(x) __attribute__ ((aligned(x)))
/* dirname() for linux/mingw */
#include <libgen.h>
#endif

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
