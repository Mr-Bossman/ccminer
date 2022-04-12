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
	const  u128 Q2 = _mm_clmulepi64_si128(A, C, 0x01);
	const u128 Q3 = _mm_shuffle_epi8( *(u128*)G,_mm_srli_si128(Q2, 8));
	const u128 Q4 = _mm_xor_si128(Q2, A);
	const u128 final = _mm_xor_si128(Q3, Q4);
	return final;/// WARNING: HIGH 64 BITS CONTAIN GARBAGE
}

inline uint64_t precompReduction64(u128 A) {
	return _mm_cvtsi128_si64(precompReduction64_si128(A));
}
#define verus2_2round(randomsource,pMoveScratch,pbuf,pbuf_copy,acc,round) do {\
	const uint64_t selector = _mm_cvtsi128_si64(acc);\
	u128 *prand = randomsource + ((selector >> 5) & keyMask);\
	u128 *prandex = randomsource + ((selector >> 32) & keyMask);\
	pbuf = pbuf_copy + (selector & 3);\
	*(pMoveScratch++) = prand;\
	*(pMoveScratch++)  = prandex;\
	switch (selector & 0x1c)\
	{\
		case 0:\
		{\
			const u128 temp1 = _mm_load_si128(prandex);\
			const u128 temp2 = pbuf[(selector & 1) ? -1 : 1];\
			const u128 add1 = _mm_xor_si128(temp1, temp2);\
			const u128 clprod1 = _mm_clmulepi64_si128(add1, add1, 0x10);\
			acc = _mm_xor_si128(clprod1, acc);\
\
			const u128 tempa1 = _mm_mulhrs_epi16(acc, temp1);\
			const u128 tempa2 = _mm_xor_si128(tempa1, temp1);\
\
			const u128 temp12 = _mm_load_si128(prand);\
			_mm_store_si128(prand, tempa2);\
\
			const u128 temp22 = _mm_load_si128(pbuf);\
			const u128 add12 = _mm_xor_si128(temp12, temp22);\
			const u128 clprod12 = _mm_clmulepi64_si128(add12, add12, 0x10);\
			acc = _mm_xor_si128(clprod12, acc);\
\
			const u128 tempb1 = _mm_mulhrs_epi16(acc, temp12);\
			const u128 tempb2 = _mm_xor_si128(tempb1, temp12);\
			_mm_store_si128(prandex, tempb2);\
			break;\
		}\
		case 4:\
		{\
			const u128 temp1 = _mm_load_si128(prand);\
			const u128 temp2 = _mm_load_si128(pbuf);\
			const u128 add1 = _mm_xor_si128(temp1, temp2);\
			const u128 clprod1 = _mm_clmulepi64_si128(add1, add1, 0x10);\
			acc = _mm_xor_si128(clprod1, acc);\
			const u128 clprod2 = _mm_clmulepi64_si128(temp2, temp2, 0x10);\
			acc = _mm_xor_si128(clprod2, acc);\
\
			const u128 tempa1 = _mm_mulhrs_epi16(acc, temp1);\
			const u128 tempa2 = _mm_xor_si128(tempa1, temp1);\
\
			const u128 temp12 = _mm_load_si128(prandex);\
			_mm_store_si128(prandex, tempa2);\
\
			const u128 temp22 = pbuf[(selector & 1) ? -1 : 1];\
			const u128 add12 = _mm_xor_si128(temp12, temp22);\
			acc = _mm_xor_si128(add12, acc);\
\
			const u128 tempb1 = _mm_mulhrs_epi16(acc, temp12);\
			_mm_store_si128(prand,_mm_xor_si128(tempb1, temp12));\
			break;\
		}\
		case 8:\
		{\
			const u128 temp1 = _mm_load_si128(prandex);\
			const u128 temp2 = _mm_load_si128(pbuf);\
			const u128 add1 = _mm_xor_si128(temp1, temp2);\
			acc = _mm_xor_si128(add1, acc);\
\
			const u128 tempa1 = _mm_mulhrs_epi16(acc, temp1);\
			const u128 tempa2 = _mm_xor_si128(tempa1, temp1);\
\
			const u128 temp12 = _mm_load_si128(prand);\
			_mm_store_si128(prand, tempa2);\
\
			const u128 temp22 = pbuf[(selector & 1) ? -1 : 1];\
			const u128 add12 = _mm_xor_si128(temp12, temp22);\
			const u128 clprod12 = _mm_clmulepi64_si128(add12, add12, 0x10);\
			acc = _mm_xor_si128(clprod12, acc);\
			const u128 clprod22 = _mm_clmulepi64_si128(temp22, temp22, 0x10);\
			acc = _mm_xor_si128(clprod22, acc);\
\
			const u128 tempb1 = _mm_mulhrs_epi16(acc, temp12);\
			const u128 tempb2 = _mm_xor_si128(tempb1, temp12);\
			_mm_store_si128(prandex, tempb2);\
			break;\
		}\
		case 0xc:\
		{\
			const u128 temp1 = _mm_load_si128(prand);\
			const u128 temp2 = pbuf[(selector & 1) ? -1 : 1];\
			const u128 add1 = _mm_xor_si128(temp1, temp2);\
\
			const int32_t divisor = (uint32_t)selector;\
\
			acc = _mm_xor_si128(add1, acc);\
\
			const int64_t dividend = _mm_cvtsi128_si64(acc);\
			const u128 modulo = _mm_cvtsi32_si128(dividend % divisor);\
			acc = _mm_xor_si128(modulo, acc);\
\
			const u128 tempa1 = _mm_mulhrs_epi16(acc, temp1);\
			const u128 tempa2 = _mm_xor_si128(tempa1, temp1);\
\
			if (dividend & 1)\
			{\
				const u128 temp12 = _mm_load_si128(prandex);\
				_mm_store_si128(prandex, tempa2);\
\
				const u128 temp22 = _mm_load_si128(pbuf);\
				const u128 add12 = _mm_xor_si128(temp12, temp22);\
				const u128 clprod12 = _mm_clmulepi64_si128(add12, add12, 0x10);\
				acc = _mm_xor_si128(clprod12, acc);\
				const u128 clprod22 = _mm_clmulepi64_si128(temp22, temp22, 0x10);\
				acc = _mm_xor_si128(clprod22, acc);\
\
				const u128 tempb1 = _mm_mulhrs_epi16(acc, temp12);\
				const u128 tempb2 = _mm_xor_si128(tempb1, temp12);\
				_mm_store_si128(prand, tempb2);\
			}\
			else\
			{\
				_mm_store_si128(prand, _mm_load_si128(prandex));\
				_mm_store_si128(prandex, tempa2);\
				acc = _mm_xor_si128(_mm_load_si128(pbuf), acc);\
			}\
			break;\
		}\
		case 0x10:\
		{\
			const u128 *rc = prand;\
			u128 tmp;\
\
			u128 temp1 = pbuf[(selector & 1) ? -1 : 1];\
			u128 temp2 = _mm_load_si128(pbuf);\
\
			AES2(temp1, temp2, 0);\
			MIX2(temp1, temp2);\
\
			AES2(temp1, temp2, 4);\
			MIX2(temp1, temp2);\
\
			AES2(temp1, temp2, 8);\
			MIX2(temp1, temp2);\
\
			acc = _mm_xor_si128(temp2, _mm_xor_si128(temp1, acc));\
\
			const u128 tempa1 = _mm_load_si128(prand);\
			const u128 tempa2 = _mm_mulhrs_epi16(acc, tempa1);\
\
			_mm_store_si128(prand, _mm_load_si128(prandex));\
			_mm_store_si128(prandex, _mm_xor_si128(tempa1, tempa2));\
\
			break;\
		}\
		case 0x14:\
		{\
			const u128 *buftmp = &pbuf[(selector & 1) ? -1 : 1];\
			u128 tmp;\
\
			uint64_t rounds = selector >> 61;\
			u128 *rc = prand;\
			uint64_t aesroundoffset = 0;\
			u128 onekey;\
\
			do\
			{\
				if (selector & (((uint64_t)0x10000000) << rounds))\
				{\
					const u128 temp2 = _mm_load_si128(rounds & 1 ? pbuf : buftmp);\
					const u128 add1 = _mm_xor_si128(rc[0], temp2); rc++;\
					const u128 clprod1 = _mm_clmulepi64_si128(add1, add1, 0x10);\
					acc = _mm_xor_si128(clprod1, acc);\
				}\
				else\
				{\
					onekey = _mm_load_si128(rc++);\
					u128 temp2 = _mm_load_si128(rounds & 1 ? buftmp : pbuf);\
					AES2(onekey, temp2, aesroundoffset);\
					aesroundoffset += 4;\
					MIX2(onekey, temp2);\
					acc = _mm_xor_si128(onekey, acc);\
					acc = _mm_xor_si128(temp2, acc);\
				}\
			} while (rounds--);\
\
			const u128 tempa1 = _mm_load_si128(prand);\
			const u128 tempa2 = _mm_mulhrs_epi16(acc, tempa1);\
			const u128 tempa3 = _mm_xor_si128(tempa1, tempa2);\
\
			const u128 tempa4 = _mm_load_si128(prandex);\
			_mm_store_si128(prandex, tempa3);\
			_mm_store_si128(prand, tempa4);\
			break;\
		}\
		case 0x18:\
		{\
			const u128 *buftmp = &pbuf[(selector & 1) ? -1 : 1];\
\
			uint64_t rounds = selector >> 61;\
			u128 *rc = prand;\
			u128 onekey;\
\
			do\
			{\
				if (selector & (((uint64_t)0x10000000) << rounds))\
				{\
					const u128 temp2 = _mm_load_si128(rounds & 1 ? pbuf : buftmp);\
					onekey = _mm_xor_si128(rc[0], temp2); rc++;\
					const int32_t divisor = (uint32_t)selector;\
					const int64_t dividend = _mm_cvtsi128_si64(onekey);\
					const u128 modulo = _mm_cvtsi32_si128(dividend % divisor);\
					acc = _mm_xor_si128(modulo, acc);\
				}\
				else\
				{\
					u128 temp2 = _mm_load_si128(rounds & 1 ? buftmp : pbuf);\
					const u128 add1 = _mm_xor_si128(rc[0], temp2); rc++;\
					onekey = _mm_clmulepi64_si128(add1, add1, 0x10);\
					const u128 clprod2 = _mm_mulhrs_epi16(acc, onekey);\
					acc = _mm_xor_si128(clprod2, acc);\
				}\
			} while (rounds--);\
\
			const u128 tempa3 = _mm_load_si128(prandex);\
\
			_mm_store_si128(prandex, onekey);\
			_mm_store_si128(prand, _mm_xor_si128(tempa3, acc));\
			break;\
		}\
		case 0x1c:\
		{\
			const u128 temp1 = _mm_load_si128(pbuf);\
			const u128 temp2 = _mm_load_si128(prandex);\
			const u128 add1 = _mm_xor_si128(temp1, temp2);\
			const u128 clprod1 = _mm_clmulepi64_si128(add1, add1, 0x10);\
			acc = _mm_xor_si128(clprod1, acc);\
\
			const u128 tempa1 = _mm_mulhrs_epi16(acc, temp2);\
			const u128 tempa2 = _mm_xor_si128(tempa1, temp2);\
\
			const u128 tempa3 = _mm_load_si128(prand);\
			_mm_store_si128(prand, tempa2);\
\
			acc = _mm_xor_si128(tempa3, acc);\
			const u128 temp4 = pbuf[(selector & 1) ? -1 : 1];\
			acc = _mm_xor_si128(temp4, acc);\
			const u128 tempb1 = _mm_mulhrs_epi16(acc, tempa3);\
			*prandex = _mm_xor_si128(tempb1, tempa3);\
			break;\
		}\
	}\
} while(0)

inline u128 __verusclmulwithoutreduction64alignedrepeatv2_2(u128 *randomsource, const u128 buf[4], u128 **pMoveScratch)
{
	const u128 pbuf_copy[4] = { _mm_xor_si128(buf[0], buf[2]), _mm_xor_si128(buf[1], buf[3]), buf[2], buf[3] };
	const u128 *pbuf;

	// divide key mask by 16 from bytes to u128
	//keyMask >>= 4;

	// the random buffer must have at least 32 16 byte dwords after the keymask to work with this
	// algorithm. we take the value from the last element inside the keyMask + 2, as that will never
	// be used to xor into the accumulator before it is hashed with other values first
	u128 acc = _mm_load_si128(randomsource + (keyMask + 2));
	#if 1
	for (int64_t i = 0; i < 32; i++)
	{
		verus2_2round(randomsource,pMoveScratch,pbuf,pbuf_copy,acc,i);
	}
	#else
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,0);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,1);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,2);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,3);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,4);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,5);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,6);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,7);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,8);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,9);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,10);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,11);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,12);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,13);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,14);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,15);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,16);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,17);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,18);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,19);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,20);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,21);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,22);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,23);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,24);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,25);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,26);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,27);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,28);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,29);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,30);
	verus2_2round(randomsource,fixrand,fixrandex,g_prand,g_prandex,pbuf,pbuf_copy,acc,31);
	#endif
	return acc;
}

uint64_t verusclhashv2_2(void * random, const unsigned char buf[64], u128 **pMoveScratch) {
	u128  acc = __verusclmulwithoutreduction64alignedrepeatv2_2((u128 *)random, (const u128 *)buf, pMoveScratch);
	acc = _mm_xor_si128(acc, *(u128*)lazyLengthHashC);
	return precompReduction64(acc);
}
