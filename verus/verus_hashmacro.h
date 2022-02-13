#ifndef VERUS_HASH_MACRO_H
#define VERUS_HASH_MACRO_H


#define CASE0() {\
	const u128 temp1 = _mm_load_si128(prandex);\
	const u128 temp2 = _mm_load_si128(buftmp);\
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
	}

#define CASE1() {\
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
	const u128 temp22 = _mm_load_si128(buftmp);\
	const u128 add12 = _mm_xor_si128(temp12, temp22);\
	acc = _mm_xor_si128(add12, acc);\
	\
	const u128 tempb1 = _mm_mulhrs_epi16(acc, temp12);\
	_mm_store_si128(prand,_mm_xor_si128(tempb1, temp12));\
	}

#define CASE2() {\
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
	const u128 temp22 = _mm_load_si128(buftmp);\
	const u128 add12 = _mm_xor_si128(temp12, temp22);\
	const u128 clprod12 = _mm_clmulepi64_si128(add12, add12, 0x10);\
	acc = _mm_xor_si128(clprod12, acc);\
	const u128 clprod22 = _mm_clmulepi64_si128(temp22, temp22, 0x10);\
	acc = _mm_xor_si128(clprod22, acc);\
	\
	const u128 tempb1 = _mm_mulhrs_epi16(acc, temp12);\
	const u128 tempb2 = _mm_xor_si128(tempb1, temp12);\
	_mm_store_si128(prandex, tempb2);\
	}

#define CASE3() {\
	const u128 temp1 = _mm_load_si128(prand);\
	const u128 temp2 = _mm_load_si128(buftmp);\
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
	if (dividend & 1) {\
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
	} else {\
		_mm_store_si128(prand, _mm_load_si128(prandex));\
		_mm_store_si128(prandex, tempa2);\
		acc = _mm_xor_si128(_mm_load_si128(pbuf), acc);\
	}\
	}

#define CASE4() {\
	const u128 *rc = prand;\
	u128 tmp;\
	\
	u128 temp1 = _mm_load_si128(buftmp);\
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
	}

#define CASE5() {\
	u128 tmp;\
	\
	uint64_t rounds = selector >> 61;/*max is 7 */\
	u128 *rc = prand;\
	uint64_t aesroundoffset = 0;\
	u128 onekey;\
	const uint64_t sel = (selector>>28)&0xff;\
	\
	do {\
		if ((sel>>rounds) & 1) {\
			const u128 temp2 = _mm_load_si128(rounds & 1 ? pbuf : buftmp);\
			const u128 add1 = _mm_xor_si128(rc[0], temp2); rc++;\
			const u128 clprod1 = _mm_clmulepi64_si128(add1, add1, 0x10);\
			acc = _mm_xor_si128(clprod1, acc);\
		} else {\
			onekey = _mm_load_si128(rc++);\
			u128 temp2 = _mm_load_si128(rounds & 1 ? buftmp : pbuf);\
			AES2(onekey, temp2, aesroundoffset);\
			aesroundoffset += 4;\
			MIX2(onekey, temp2);\
			acc = _mm_xor_si128(onekey, acc);\
			acc = _mm_xor_si128(temp2, acc);\
		}\
	} while (rounds--);\
	const u128 tempa1 = _mm_load_si128(prand);\
	const u128 tempa2 = _mm_mulhrs_epi16(acc, tempa1);\
	const u128 tempa3 = _mm_xor_si128(tempa1, tempa2);\
	\
	const u128 tempa4 = _mm_load_si128(prandex);\
	_mm_store_si128(prandex, tempa3);\
	_mm_store_si128(prand, tempa4);\
	}

#define CASE6() {\
	uint64_t rounds = selector >> 61;;/*max is 7 */\
	u128 *rc = prand;\
	u128 onekey;\
	const uint64_t sel = (selector>>28)&0xff;\
	\
	do {\
		if ((sel>>rounds) & 1) {\
			const u128 temp2 = _mm_load_si128(rounds & 1 ? pbuf : buftmp);\
			onekey = _mm_xor_si128(rc[0], temp2); rc++;\
			const int32_t divisor = (uint32_t)selector;\
			const int64_t dividend = _mm_cvtsi128_si64(onekey);\
			const u128 modulo = _mm_cvtsi32_si128(dividend % divisor);\
			acc = _mm_xor_si128(modulo, acc);\
		} else {\
			u128 temp2 = _mm_load_si128(rounds & 1 ? buftmp : pbuf);\
			const u128 add1 = _mm_xor_si128(rc[0], temp2); rc++;\
			onekey = _mm_clmulepi64_si128(add1, add1, 0x10);\
			const u128 clprod2 = _mm_mulhrs_epi16(acc, onekey);\
			acc = _mm_xor_si128(clprod2, acc);\
		}\
	} while (rounds--);\
	const u128 tempa3 = _mm_load_si128(prandex);\
	_mm_store_si128(prandex, onekey);\
	_mm_store_si128(prand, _mm_xor_si128(tempa3, acc));\
	}

#define CASE7() {\
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
	const u128 temp4 = _mm_load_si128(buftmp);\
	acc = _mm_xor_si128(temp4, acc);\
	const u128 tempb1 = _mm_mulhrs_epi16(acc, tempa3);\
	*prandex = _mm_xor_si128(tempb1, tempa3);\
	}


#define verus2_2round() {\
	const uint64_t selector = _mm_cvtsi128_si64(acc);\
	uint32_t prand_idx = (selector >> 5) & keyMask;\
	uint32_t prandex_idx = (selector >> 32) & keyMask;\
	u128 *prand = randomsource + prand_idx;\
	u128 *prandex = randomsource + prandex_idx;\
	pbuf = pbuf_copy + (selector & 3);\
	const u128 *buftmp = pbuf_copy + ((selector & 3)^1);\
	_mm_store_si128(g_prand++, *prand);\
	_mm_store_si128(g_prandex++, *prandex);\
	*fixrand++ = prand_idx;\
	*fixrandex++ = prandex_idx;\
	switch (selector & 0x1c)/* slower: ((selector>>2) & 7) */\
	{\
		case 0:\
			CASE0()\
			break;\
		case 4:\
			CASE1()\
			break;\
		case 8:\
			CASE2()\
			break;\
		case 0xc:\
			CASE3()\
			break;\
		case 0x10:\
			CASE4()\
			break;\
		case 0x14:\
			CASE5()\
			break;\
		case 0x18:\
			CASE6()\
			break;\
		case 0x1c:\
			CASE7()\
			break;\
	}\
}

#endif /* VERUS_HASH_MACRO_H */