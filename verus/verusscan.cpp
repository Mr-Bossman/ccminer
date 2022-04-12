/**
* Equihash solver interface for ccminer (compatible with linux and windows)
* Solver taken from nheqminer, by djeZo (and NiceHash)
* tpruvot - 2017 (GPL v3)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdexcept>
#include <vector>
#define VERUS_KEY_SIZE 8832
#define VERUS_KEY_SIZE128 552
#include <miner.h>
#include "portability.h"

enum
{
	// primary actions
	SER_NETWORK = (1 << 0),
	SER_DISK = (1 << 1),
	SER_GETHASH = (1 << 2),
};
// input here is 140 for the header and 1344 for the solution (equi.cpp)
static const int PROTOCOL_VERSION = 170002;

//#include <cuda_helper.h>

#define EQNONCE_OFFSET 30 /* 27:34 */
#define NONCE_OFT EQNONCE_OFFSET

static bool init[MAX_GPUS] = { 0 };

static __thread uint32_t throughput = 0;
static u128 shuf1;
static u128 shuf2;


#ifndef htobe32
#define htobe32(x) swab32(x)
#endif

extern "C" inline void GenNewCLKey(unsigned char *seedBytes32, u128 *keyback)
{
	// generate a new key by chain hashing with Haraka256 from the last curbuf
	int n256blks = VERUS_KEY_SIZE >> 5;  //8832 >> 5
	int nbytesExtra = VERUS_KEY_SIZE & 0x1f;  //8832 & 0x1f
	unsigned char *pkey = (unsigned char*)keyback;
	unsigned char *psrc = seedBytes32;
	for (int i = 0; i < n256blks; i++)
	{
		haraka256(pkey, psrc);

		psrc = pkey;
		pkey += 32;
	}
	if (nbytesExtra)
	{
		unsigned char buf[32];
		haraka256(buf, psrc);
		memcpy(pkey, buf, nbytesExtra);
	}
}

extern "C" inline void FixKey(u128 **pMoveScratch)
{
	uint32_t ofs = VERUS_KEY_SIZE >> 4;
	#pragma clang loop unroll(full)
	for (u128 *pfixup = *pMoveScratch; pfixup; pfixup = *++pMoveScratch)
	{
		const u128 fixup = _mm_load_si128((u128 *)(pfixup + ofs));
		_mm_store_si128((u128 *)pfixup, fixup);
	}

}


 extern "C" inline void VerusHashHalf(void *result2, unsigned char *data, size_t len)
{
	alignas(32) unsigned char buf2[64];
	unsigned char *curBuf = (unsigned char *)result2 , *result = buf2;
	size_t curPos = 0;
	//std::fill(buf1, buf1 + sizeof(buf1), 0);


	// digest up to 32 bytes at a time
	for (size_t pos = 0; pos < len; )
	{
		size_t room = 32 - curPos;

		if (len - pos >= room)
		{
			memcpy(curBuf + 32 + curPos, data + pos, room);
			haraka512(result, curBuf);
			unsigned char *tmp = curBuf;
			curBuf = result;
			result = tmp;
			pos += room;
			curPos = 0;
		}
		else
		{
			memcpy(curBuf + 32 + curPos, data + pos, len - pos);
			curPos += len - pos;
			pos = len;
		}
	}

	//memcpy(curBuf + 47, curBuf, 16);
	((uint64_t*)(curBuf + 47))[0] = ((uint64_t*)curBuf)[0];
	((uint64_t*)(curBuf + 47))[1] = ((uint64_t*)curBuf)[1];
	curBuf[63] = curBuf[0];
	//memcpy(curBuf + 63, curBuf, 1);
	//	FillExtra((u128 *)curBuf);
//	memcpy(result2, curBuf, 64);
};




extern "C" void inline Verus2hash(unsigned char *hash, unsigned char *curBuf, uint32_t nonce,
	u128 *data_key, u128 **pMoveScratch)
{
	//uint64_t mask = VERUS_KEY_SIZE128; //552
	const u128 fill1 = _mm_shuffle_epi8(_mm_load_si128((u128 *)curBuf), shuf1);
	unsigned char ch = curBuf[0];
	_mm_store_si128((u128 *)(&curBuf[32 + 16]), fill1);
	curBuf[32 + 15] = ch;
	//	FillExtra((u128 *)curBuf);
	uint64_t intermediate;
	((uint32_t*)&curBuf[0])[8] = nonce;

	intermediate = verusclhashv2_2(data_key, curBuf, pMoveScratch);
		//FillExtra
	u128 fill2 = _mm_shuffle_epi8(_mm_loadl_epi64((u128 *)&intermediate), shuf2);
	_mm_store_si128((u128 *)(&curBuf[32 + 16]), fill2);
	curBuf[32 + 15] = *((unsigned char *)&intermediate);
	intermediate &= 511;
	haraka512_keyed(hash, curBuf, data_key + intermediate);
	FixKey(pMoveScratch);
}

extern "C" int scanhash_verus(int thr_id, struct work *work, uint32_t max_nonce, unsigned long *hashes_done)
{
	uint32_t *pdata = work->data;
	uint32_t *ptarget = work->target;

	uint8_t _ALIGN(32)  blockhash_half[64] = { 0 };
	u128 data_key[(VERUS_KEY_SIZE << 1)/sizeof(u128)] = {0};
	u128 ** pMoveScratch = (__m128i **)((unsigned char *)data_key + (VERUS_KEY_SIZE + 8192));
	uint32_t nonce_buf = 0;
	uint32_t _ALIGN(64) vhash[8] = { 0 };

	unsigned char block_41970[3] = { 0xfd, 0x40, 0x05};
	uint8_t* full_data =  work->fulldata;

	memcpy(full_data + 140, block_41970, 3);
	memcpy(full_data + 143, &work->hash_ver, 4);
	/* loads consts */
	shuf1 = _mm_setr_epi8(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0);
	shuf2 = _mm_setr_epi8(1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0);
	load_constants();


	VerusHashHalf(blockhash_half, (unsigned char*)full_data, 1487);

	GenNewCLKey((unsigned char*)blockhash_half, data_key);  //data_key a global static 2D array data_key[16][8832];
        memcpy((unsigned char *)data_key + VERUS_KEY_SIZE, data_key, 8192);
       // memset(pMoveScratch, 0, VERUS_KEY_SIZE - 8192);


	//gettimeofday(&tv_start, NULL);

	throughput = 1;
	const uint32_t Htarg = ptarget[7];
	do {

		*hashes_done = nonce_buf + throughput;
		Verus2hash((unsigned char *)vhash, (unsigned char *)blockhash_half, nonce_buf, data_key, pMoveScratch);

		if (vhash[7] <= Htarg )
		{
			*((uint32_t *)full_data + 368) = nonce_buf;
			work->valid_nonces++;
			bn_store_hash_target_ratio(vhash, work->target, work, work->valid_nonces - 1);
			work->nonces[work->valid_nonces - 1] = ((uint32_t*)full_data)[NONCE_OFT];
			break;
		}
		if ((uint64_t)throughput + (uint64_t)nonce_buf >= (uint64_t)max_nonce) {

			break;
		}
		nonce_buf += throughput;

	} while (!work_restart[thr_id].restart);

	pdata[NONCE_OFT] = ((uint32_t*)full_data)[NONCE_OFT] + 1;
	return work->valid_nonces;
}

// cleanup
void free_verushash(int thr_id)
{
	if (!init[thr_id])
		return;



	init[thr_id] = false;
}
