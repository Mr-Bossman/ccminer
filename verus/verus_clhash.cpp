/*
* This uses veriations of the clhash algorithm for Verus Coin, licensed
* with the Apache-2.0 open source license.
*
* Copyright (c) 2018 Michael Toutonghi
* Distributed under the Apache 2.0 software license, available in the original form for clhash
* here: https://github.com/lemire/clhash/commit/934da700a2a54d8202929a826e2763831bd43cf7#diff-9879d6db96fd29134fc802214163b95a
*
* Original CLHash code and any portions herein, (C) 2017, 2018 Daniel Lemire and Owen Kaser
* Faster 64-bit universal hashing
* using carry-less multiplications, Journal of Cryptographic Engineering (to appear)
*
* Best used on recent x64 processors (Haswell or better).
*
* This implements an intermediate step in the last part of a Verus block hash. The intent of this step
* is to more effectively equalize FPGAs over GPUs and CPUs.
*
**/

#include <assert.h>
#include <string.h>
#include "portability.h"
#include "verus_hashmacro.h"

#define keyMask 511
// multiply the length and the some key, no modulo
inline u128 lazyLengthHash(uint64_t keylength, uint64_t length) {
	const u128 lengthvector = _mm_set_epi64x(keylength, length);
	const u128 clprod1 = _mm_clmulepi64_si128(lengthvector, lengthvector, 0x10);
	return clprod1;
}

const uint64_t _ALIGN(16) Q[2] = {(1U << 4) + (1U << 3) + (1U << 1) + (1U << 0),0};
const uint64_t _ALIGN(16) lazyLengthHashC[2] = {0x10000,0};
const uint8_t _ALIGN(16) G[16] = {0, 27, 54, 45, 108, 119, 90, 65, (uint8_t)216, (uint8_t)195, (uint8_t)238, (uint8_t)245, (uint8_t)180, (uint8_t)175, (uint8_t)130, (uint8_t)153};

// modulo reduction to 64-bit value. The high 64 bits contain garbage, see precompReduction64
inline u128 precompReduction64_si128(u128 A) {

	//const u128 C = _mm_set_epi64x(1U,(1U<<4)+(1U<<3)+(1U<<1)+(1U<<0)); // C is the irreducible poly. (64,4,3,1,0)
	const u128 C = *(u128*)Q;
	const u128 Q2 = _mm_clmulepi64_si128(A, C, 0x01);
	const u128 Q3 = _mm_shuffle_epi8( *(u128*)G,_mm_srli_si128(Q2, 8));
	const u128 Q4 = _mm_xor_si128(Q2, A);
	const u128 final = _mm_xor_si128(Q3, Q4);
	return final;/// WARNING: HIGH 64 BITS CONTAIN GARBAGE
}

inline uint64_t precompReduction64(u128 A) {
	return _mm_cvtsi128_si64(precompReduction64_si128(A));
}

inline uint64_t __verusclmulwithoutreduction64alignedrepeatv2_2(u128 *randomsource, const u128 buf[4], uint32_t *fixrand, uint32_t *fixrandex, u128 *g_prand, u128 *g_prandex)
{
	const u128 pbuf_copy[4] = { _mm_xor_si128(buf[0], buf[2]), _mm_xor_si128(buf[1], buf[3]), buf[2], buf[3] };
	const u128 *pbuf;

	// divide key mask by 16 from bytes to u128
	//keyMask >>= 4;
	const uint32_t* tmpramx = fixrand;
	const u128* tmpgramx = g_prand;
	// the random buffer must have at least 32 16 byte dwords after the keymask to work with this
	// algorithm. we take the value from the last element inside the keyMask + 2, as that will never
	// be used to xor into the accumulator before it is hashed with other values first
	u128 acc = _mm_load_si128(randomsource + (keyMask + 2));

	for (int64_t round = 0; round < 32; round+=2){ /* we dont use round in macros anymore */
		verus2_2round()
		verus2_2round()
	}
	return 	precompReduction64(_mm_xor_si128(acc, *(u128*)lazyLengthHashC));
}

uint64_t verusclhashv2_2(void * random, const unsigned char buf[64],uint32_t *fixrand, uint32_t *fixrandex, u128 *g_prand, u128 *g_prandex) {
	return __verusclmulwithoutreduction64alignedrepeatv2_2((u128 *)random, (const u128 *)buf, fixrand, fixrandex,g_prand,g_prandex);
}
