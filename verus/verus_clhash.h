/*
 * This uses veriations of the clhash algorithm for Verus Coin, licensed
 * with the Apache-2.0 open source license.
 * 
 * Copyright (c) 2018 Michael Toutonghi
 * Distributed under the Apache 2.0 software license, available in the original form for clhash
 * here: https://github.com/lemire/clhash/commit/934da700a2a54d8202929a826e2763831bd43cf7#diff-9879d6db96fd29134fc802214163b95a
 * 
 * CLHash is a very fast hashing function that uses the
 * carry-less multiplication and SSE instructions.
 *
 * Original CLHash code (C) 2017, 2018 Daniel Lemire and Owen Kaser
 * Faster 64-bit universal hashing
 * using carry-less multiplications, Journal of Cryptographic Engineering (to appear)
 *
 * Best used on recent x64 processors (Haswell or better).
 *
 **/

#ifndef INCLUDE_VERUS_CLHASH_H
#define INCLUDE_VERUS_CLHASH_H





#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif
#ifndef NCPUID
#ifdef _WIN32
#define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)

	typedef unsigned char u_char;

typedef unsigned char u_char;

#endif
enum {
    // Verus Key size must include the equivalent size of a Haraka key
    // after the first part.
    // Any excess over a power of 2 will not get mutated, and any excess over
    // power of 2 + Haraka sized key will not be used
	VERUSKEYSIZE = 1024 * 8 + (40 * 16),
	VERUSHHASH_SOLUTION_VERSION = 1
};


#endif /* NCPUID */
uint64_t verusclhashv2_1(void * random, const unsigned char buf[64], uint64_t keyMask, uint32_t *fixrand, uint32_t *fixrandex,
	u128 *g_prand, u128 *g_prandex);
uint64_t verusclhashv2_2(void * random, const unsigned char buf[64], uint64_t keyMask, uint32_t *fixrand, uint32_t *fixrandex,
	u128 *g_prand, u128 *g_prandex);

#ifdef __cplusplus
} // extern "C"
#include <vector>
#include <string>

// special high speed hasher for VerusHash 2.0

#endif // #ifdef __cplusplus

#endif // INCLUDE_VERUS_CLHASH_H
