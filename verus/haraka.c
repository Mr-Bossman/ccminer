/*
The MIT License (MIT)

Copyright (c) 2016 kste

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Optimized Implementations for Haraka256 and Haraka512
*/

#include <stdio.h>
#include "haraka.h"
#include <stdint.h>
#include <stdalign.h>

alignas(128) uint8_t _rc[40*16];
alignas(128) uint8_t _rc0[40*16] = {0};

#define rc0(i) *((__m128i *)(&_rc0[i*16]))

void load_constants() {
  rc(0) = _mm_set_epi32(0x0684704c,0xe620c00a,0xb2c5fef0,0x75817b9d);
  rc(1) = _mm_set_epi32(0x8b66b4e1,0x88f3a06b,0x640f6ba4,0x2f08f717);
  rc(2) = _mm_set_epi32(0x3402de2d,0x53f28498,0xcf029d60,0x9f029114);
  rc(3) = _mm_set_epi32(0x0ed6eae6,0x2e7b4f08,0xbbf3bcaf,0xfd5b4f79);
  rc(4) = _mm_set_epi32(0xcbcfb0cb,0x4872448b,0x79eecd1c,0xbe397044);
  rc(5) = _mm_set_epi32(0x7eeacdee,0x6e9032b7,0x8d5335ed,0x2b8a057b);
  rc(6) = _mm_set_epi32(0x67c28f43,0x5e2e7cd0,0xe2412761,0xda4fef1b);
  rc(7) = _mm_set_epi32(0x2924d9b0,0xafcacc07,0x675ffde2,0x1fc70b3b);
  rc(8) = _mm_set_epi32(0xab4d63f1,0xe6867fe9,0xecdb8fca,0xb9d465ee);
  rc(9) = _mm_set_epi32(0x1c30bf84,0xd4b7cd64,0x5b2a404f,0xad037e33);
  rc(10) = _mm_set_epi32(0xb2cc0bb9,0x941723bf,0x69028b2e,0x8df69800);
  rc(11) = _mm_set_epi32(0xfa0478a6,0xde6f5572,0x4aaa9ec8,0x5c9d2d8a);
  rc(12) = _mm_set_epi32(0xdfb49f2b,0x6b772a12,0x0efa4f2e,0x29129fd4);
  rc(13) = _mm_set_epi32(0x1ea10344,0xf449a236,0x32d611ae,0xbb6a12ee);
  rc(14) = _mm_set_epi32(0xaf044988,0x4b050084,0x5f9600c9,0x9ca8eca6);
  rc(15) = _mm_set_epi32(0x21025ed8,0x9d199c4f,0x78a2c7e3,0x27e593ec);
  rc(16) = _mm_set_epi32(0xbf3aaaf8,0xa759c9b7,0xb9282ecd,0x82d40173);
  rc(17) = _mm_set_epi32(0x6260700d,0x6186b017,0x37f2efd9,0x10307d6b);
  rc(18) = _mm_set_epi32(0x5aca45c2,0x21300443,0x81c29153,0xf6fc9ac6);
  rc(19) = _mm_set_epi32(0x9223973c,0x226b68bb,0x2caf92e8,0x36d1943a);
  rc(20) = _mm_set_epi32(0xd3bf9238,0x225886eb,0x6cbab958,0xe51071b4);
  rc(21) = _mm_set_epi32(0xdb863ce5,0xaef0c677,0x933dfddd,0x24e1128d);
  rc(22) = _mm_set_epi32(0xbb606268,0xffeba09c,0x83e48de3,0xcb2212b1);
  rc(23) = _mm_set_epi32(0x734bd3dc,0xe2e4d19c,0x2db91a4e,0xc72bf77d);
  rc(24) = _mm_set_epi32(0x43bb47c3,0x61301b43,0x4b1415c4,0x2cb3924e);
  rc(25) = _mm_set_epi32(0xdba775a8,0xe707eff6,0x03b231dd,0x16eb6899);
  rc(26) = _mm_set_epi32(0x6df3614b,0x3c755977,0x8e5e2302,0x7eca472c);
  rc(27) = _mm_set_epi32(0xcda75a17,0xd6de7d77,0x6d1be5b9,0xb88617f9);
  rc(28) = _mm_set_epi32(0xec6b43f0,0x6ba8e9aa,0x9d6c069d,0xa946ee5d);
  rc(29) = _mm_set_epi32(0xcb1e6950,0xf957332b,0xa2531159,0x3bf327c1);
  rc(30) = _mm_set_epi32(0x2cee0c75,0x00da619c,0xe4ed0353,0x600ed0d9);
  rc(31) = _mm_set_epi32(0xf0b1a5a1,0x96e90cab,0x80bbbabc,0x63a4a350);
  rc(32) = _mm_set_epi32(0xae3db102,0x5e962988,0xab0dde30,0x938dca39);
  rc(33) = _mm_set_epi32(0x17bb8f38,0xd554a40b,0x8814f3a8,0x2e75b442);
  rc(34) = _mm_set_epi32(0x34bb8a5b,0x5f427fd7,0xaeb6b779,0x360a16f6);
  rc(35) = _mm_set_epi32(0x26f65241,0xcbe55438,0x43ce5918,0xffbaafde);
  rc(36) = _mm_set_epi32(0x4ce99a54,0xb9f3026a,0xa2ca9cf7,0x839ec978);
  rc(37) = _mm_set_epi32(0xae51a51a,0x1bdff7be,0x40c06e28,0x22901235);
  rc(38) = _mm_set_epi32(0xa0c1613c,0xba7ed22b,0xc173bc0f,0x48a659cf);
  rc(39) = _mm_set_epi32(0x756acc03,0x02288288,0x4ad6bdfd,0xe9c59da1);
}

void test_implementations() {
  unsigned char *in = (unsigned char *)calloc(64*8, sizeof(unsigned char));
  unsigned char *out256 = (unsigned char *)calloc(32*8, sizeof(unsigned char));
  unsigned char *out512 = (unsigned char *)calloc(32*8, sizeof(unsigned char));
  unsigned char testvector256[32] = {0x80, 0x27, 0xcc, 0xb8, 0x79, 0x49, 0x77, 0x4b,
                                     0x78, 0xd0, 0x54, 0x5f, 0xb7, 0x2b, 0xf7, 0x0c,
                                     0x69, 0x5c, 0x2a, 0x09, 0x23, 0xcb, 0xd4, 0x7b,
                                     0xba, 0x11, 0x59, 0xef, 0xbf, 0x2b, 0x2c, 0x1c};

 unsigned char testvector512[32] = {0xbe, 0x7f, 0x72, 0x3b, 0x4e, 0x80, 0xa9, 0x98,
                                    0x13, 0xb2, 0x92, 0x28, 0x7f, 0x30, 0x6f, 0x62,
                                    0x5a, 0x6d, 0x57, 0x33, 0x1c, 0xae, 0x5f, 0x34,
                                    0xdd, 0x92, 0x77, 0xb0, 0x94, 0x5b, 0xe2, 0xaa};



  int i;

  // Input for testvector
  for(i = 0; i < 512; i++) {
    in[i] = i % 64;
  }

  load_constants();
  haraka512_8x(out512, in);

  // Verify output
  for(i = 0; i < 32; i++) {
    if (out512[i % 32] != testvector512[i]) {
      printf("Error: testvector incorrect.\n");
      return;
    }
  }

  free(in);
  free(out256);
  free(out512);
}

void haraka256(unsigned char *out, const unsigned char *in) {
	__m128i s0, s1, tmp;

	s0 = LOAD(in);
	s1 = LOAD(in + 16);

	AES2(s0, s1, 0);
	MIX2(s0, s1);

	AES2(s0, s1, 4);
	MIX2(s0, s1);

	AES2(s0, s1, 8);
	MIX2(s0, s1);

	AES2(s0, s1, 12);
	MIX2(s0, s1);

	AES2(s0, s1, 16);
	MIX2(s0, s1);

	s0 = _mm_xor_si128(s0, LOAD(in));
	s1 = _mm_xor_si128(s1, LOAD(in + 16));

	STORE(out, s0);
	STORE(out + 16, s1);
}

void haraka256_keyed(unsigned char *out, const unsigned char *in, const u128 *rc) {
  __m128i s0, s1, tmp;

  s0 = LOAD(in);
  s1 = LOAD(in + 16);


  s0 = _mm_xor_si128(s0, LOAD(in));
  s1 = _mm_xor_si128(s1, LOAD(in + 16));

  STORE(out, s0);
  STORE(out + 16, s1);
}

void haraka256_4x(unsigned char *out, const unsigned char *in) {
  __m128i s00, s01, s10, s11, s20, s21, s30, s31, tmp;

  s00 = LOAD(in);
  s01 = LOAD(in + 16);
  s10 = LOAD(in + 32);
  s11 = LOAD(in + 48);
  s20 = LOAD(in + 64);
  s21 = LOAD(in + 80);
  s30 = LOAD(in + 96);


  MIX2(s00, s01);
  MIX2(s10, s11);
  MIX2(s20, s21);
  MIX2(s30, s31);

  // Feed Forward
  s00 = _mm_xor_si128(s00, LOAD(in));
  s01 = _mm_xor_si128(s01, LOAD(in + 16));
  s10 = _mm_xor_si128(s10, LOAD(in + 32));
  s11 = _mm_xor_si128(s11, LOAD(in + 48));
  s20 = _mm_xor_si128(s20, LOAD(in + 64));
  s21 = _mm_xor_si128(s21, LOAD(in + 80));
  s30 = _mm_xor_si128(s30, LOAD(in + 96));
  s31 = _mm_xor_si128(s31, LOAD(in + 112));

  STORE(out, s00);
  STORE(out + 16, s01);
  STORE(out + 32, s10);
  STORE(out + 48, s11);
  STORE(out + 64, s20);
  STORE(out + 80, s21);
  STORE(out + 96, s30);
  STORE(out + 112, s31);
}

void haraka256_8x(unsigned char *out, const unsigned char *in) {
  // This is faster on Skylake, the code below is faster on Haswell.
  haraka256_4x(out, in);
  haraka256_4x(out + 128, in + 128);
  return;
  // __m128i s82, tmp;
  //
  // int i;
  //
  // s00 = LOAD(in);
  // s01 = LOAD(in + 16);
  // s10 = LOAD(in + 32);
  // s11 = LOAD(in + 48);
  // s20 = LOAD(in + 64);
  // s21 = LOAD(in + 80);
  // s30 = LOAD(in + 96);
  // s31 = LOAD(in + 112);
  // s40 = LOAD(in + 128);
  // s41 = LOAD(in + 144);
  // s50 = LOAD(in + 160);
  // s51 = LOAD(in + 176);
  // s60 = LOAD(in + 192);
  // s61 = LOAD(in + 208);
  // s70 = LOAD(in + 224);
  // s71 = LOAD(in + 240);
  //
  // // Round 1
  // AES2_8x(s0, s1, s2, s3, s4, s5, s6, s7, 0);
  //
  // MIX2(s00, s01);
  // MIX2(s10, s11);
  // MIX2(s20, s21);
  // MIX2(s30, s31);
  // MIX2(s40, s41);
  // MIX2(s50, s51);
  // MIX2(s60, s61);
  // MIX2(s70, s71);
  //
  //
  // // Round 2
  // AES2_8x(s0, s1, s2, s3, s4, s5, s6, s7, 4);
  //
  // MIX2(s00, s01);
  // MIX2(s10, s11);
  // MIX2(s20, s21);
  // MIX2(s30, s31);
  // MIX2(s40, s41);
  // MIX2(s50, s51);
  // MIX2(s60, s61);
  // MIX2(s70, s71);
  //
  // // Round 3
  // AES2_8x(s0, s1, s2, s3, s4, s5, s6, s7, 8);
  //
  // MIX2(s00, s01);
  // MIX2(s10, s11);
  // MIX2(s20, s21);
  // MIX2(s30, s31);
  // MIX2(s40, s41);
  // MIX2(s50, s51);
  // MIX2(s60, s61);
  // MIX2(s70, s71);
  //
  // // Round 4
  // AES2_8x(s0, s1, s2, s3, s4, s5, s6, s7, 12);
  //
  // MIX2(s00, s01);
  // MIX2(s10, s11);
  // MIX2(s20, s21);
  // MIX2(s30, s31);
  // MIX2(s40, s41);
  // MIX2(s50, s51);
  // MIX2(s60, s61);
  // MIX2(s70, s71);
  //
  // // Round 5
  // AES2_8x(s0, s1, s2, s3, s4, s5, s6, s7, 16);
  //
  // MIX2(s00, s01);
  // MIX2(s10, s11);
  // MIX2(s20, s21);
  // MIX2(s30, s31);
  // MIX2(s40, s41);
  // MIX2(s50, s51);
  // MIX2(s60, s61);
  // MIX2(s70, s71);
  //
  // // Feed Forward
  // s00 = _mm_xor_si128(s00, LOAD(in));
  // s01 = _mm_xor_si128(s01, LOAD(in + 16));
  // s10 = _mm_xor_si128(s10, LOAD(in + 32));
  // s11 = _mm_xor_si128(s11, LOAD(in + 48));
  // s20 = _mm_xor_si128(s20, LOAD(in + 64));
  // s21 = _mm_xor_si128(s21, LOAD(in + 80));
  // s30 = _mm_xor_si128(s30, LOAD(in + 96));
  // s31 = _mm_xor_si128(s31, LOAD(in + 112));
  // s40 = _mm_xor_si128(s40, LOAD(in + 128));
  // s41 = _mm_xor_si128(s41, LOAD(in + 144));
  // s50 = _mm_xor_si128(s50, LOAD(in + 160));
  // s51 = _mm_xor_si128(s51, LOAD(in + 176));
  // s60 = _mm_xor_si128(s60, LOAD(in + 192));
  // s61 = _mm_xor_si128(s61, LOAD(in + 208));
  // s70 = _mm_xor_si128(s70, LOAD(in + 224));
  // s71 = _mm_xor_si128(s71, LOAD(in + 240));
  //
  // STORE(out, s00);
  // STORE(out + 16, s01);
  // STORE(out + 32, s10);
  // STORE(out + 48, s11);
  // STORE(out + 64, s20);
  // STORE(out + 80, s21);
  // STORE(out + 96, s30);
  // STORE(out + 112, s31);
  // STORE(out + 128, s40);
  // STORE(out + 144, s41);
  // STORE(out + 160, s50);
  // STORE(out + 176, s51);
  // STORE(out + 192, s60);
  // STORE(out + 208, s61);
  // STORE(out + 224, s70);
  // STORE(out + 240, s71);
}

void haraka512(unsigned char *out, const unsigned char *in) {
  u128 s0, s1, s2, s3, tmp;

  s0 = LOAD(in);
  s1 = LOAD(in + 16);
  s2 = LOAD(in + 32);
  s3 = LOAD(in + 48);

  AES4(s0, s1, s2, s3, 0);
  MIX4(s0, s1, s2, s3);

  AES4(s0, s1, s2, s3, 8);
  MIX4(s0, s1, s2, s3);

  AES4(s0, s1, s2, s3, 16);
  MIX4(s0, s1, s2, s3);

  AES4(s0, s1, s2, s3, 24);
  MIX4(s0, s1, s2, s3);

  AES4(s0, s1, s2, s3, 32);
  MIX4(s0, s1, s2, s3);

  s0 = _mm_xor_si128(s0, LOAD(in));
  s1 = _mm_xor_si128(s1, LOAD(in + 16));
  s2 = _mm_xor_si128(s2, LOAD(in + 32));
  s3 = _mm_xor_si128(s3, LOAD(in + 48));

  TRUNCSTORE(out, s0, s1, s2, s3);
}

void haraka512_zero(unsigned char *out, const unsigned char *in) {
  u128 s0, s1, s2, s3, tmp;

  s0 = LOAD(in);
  s1 = LOAD(in + 16);
  s2 = LOAD(in + 32);
  s3 = LOAD(in + 48);

  AES4_zero(s0, s1, s2, s3, 0);
  MIX4(s0, s1, s2, s3);

  AES4_zero(s0, s1, s2, s3, 8);
  MIX4(s0, s1, s2, s3);

  AES4_zero(s0, s1, s2, s3, 16);
  MIX4(s0, s1, s2, s3);

  AES4_zero(s0, s1, s2, s3, 24);
  MIX4(s0, s1, s2, s3);

  AES4_zero(s0, s1, s2, s3, 32);
  MIX4(s0, s1, s2, s3);

  s0 = _mm_xor_si128(s0, LOAD(in));
  s1 = _mm_xor_si128(s1, LOAD(in + 16));
  s2 = _mm_xor_si128(s2, LOAD(in + 32));
  s3 = _mm_xor_si128(s3, LOAD(in + 48));

  TRUNCSTORE(out, s0, s1, s2, s3);
}

void haraka512_keyed(unsigned char *out, const unsigned char *in, const u128 *rc) {
  u128 s0, s1, s2, s3, tmp;

  s0 = LOAD(in);
  s1 = LOAD(in + 16);
  s2 = LOAD(in + 32);
  s3 = LOAD(in + 48);

  AES4(s0, s1, s2, s3, 0);
  MIX4(s0, s1, s2, s3);

  AES4(s0, s1, s2, s3, 8);
  MIX4(s0, s1, s2, s3);

  AES4(s0, s1, s2, s3, 16);
  MIX4(s0, s1, s2, s3);

  AES4(s0, s1, s2, s3, 24);
  MIX4_LAST(s0, s1, s2, s3);

  AES4_LAST(s0, s1, s2, s3, 32);


 // s0 = _mm_xor_si128(s0, LOAD(in));
 // s1 = _mm_xor_si128(s1, LOAD(in + 16));
 // s2 = _mm_xor_si128(s2, LOAD(in + 32));
 // s3 = _mm_xor_si128(s0, LOAD(in + 48));
  ((uint32_t*)&out[0])[7] = ((uint32_t*)&s0)[10] ^ ((uint32_t*)&in[52])[0];

  //TRUNCSTORE(out, s0,s1, s2, s3);
}

void haraka512_4x(unsigned char *out, const unsigned char *in) {
  u128 s00, s01, s02, s03, s10, s11, s12, s13, s20, s21, s22, s23, s30, s31, s32, s33, tmp;

  s00 = LOAD(in);
  s01 = LOAD(in + 16);
  s02 = LOAD(in + 32);
  s03 = LOAD(in + 48);
  s10 = LOAD(in + 64);
  s11 = LOAD(in + 80);
  s12 = LOAD(in + 96);
  s13 = LOAD(in + 112);
  s20 = LOAD(in + 128);
  s21 = LOAD(in + 144);
  s22 = LOAD(in + 160);
  s23 = LOAD(in + 176);
  s30 = LOAD(in + 192);
  s31 = LOAD(in + 208);
  s32 = LOAD(in + 224);
  s33 = LOAD(in + 240);

  AES4_4x(s0, s1, s2, s3, 0);
  MIX4(s00, s01, s02, s03);
  MIX4(s10, s11, s12, s13);
  MIX4(s20, s21, s22, s23);
  MIX4(s30, s31, s32, s33);

  AES4_4x(s0, s1, s2, s3, 8);
  MIX4(s00, s01, s02, s03);
  MIX4(s10, s11, s12, s13);
  MIX4(s20, s21, s22, s23);
  MIX4(s30, s31, s32, s33);

  AES4_4x(s0, s1, s2, s3, 16);
  MIX4(s00, s01, s02, s03);
  MIX4(s10, s11, s12, s13);
  MIX4(s20, s21, s22, s23);
  MIX4(s30, s31, s32, s33);

  AES4_4x(s0, s1, s2, s3, 24);
  MIX4(s00, s01, s02, s03);
  MIX4(s10, s11, s12, s13);
  MIX4(s20, s21, s22, s23);
  MIX4(s30, s31, s32, s33);

  AES4_4x(s0, s1, s2, s3, 32);
  MIX4(s00, s01, s02, s03);
  MIX4(s10, s11, s12, s13);
  MIX4(s20, s21, s22, s23);
  MIX4(s30, s31, s32, s33);


  s00 = _mm_xor_si128(s00, LOAD(in));
  s01 = _mm_xor_si128(s01, LOAD(in + 16));
  s02 = _mm_xor_si128(s02, LOAD(in + 32));
  s03 = _mm_xor_si128(s03, LOAD(in + 48));
  s10 = _mm_xor_si128(s10, LOAD(in + 64));
  s11 = _mm_xor_si128(s11, LOAD(in + 80));
  s12 = _mm_xor_si128(s12, LOAD(in + 96));
  s13 = _mm_xor_si128(s13, LOAD(in + 112));
  s20 = _mm_xor_si128(s20, LOAD(in + 128));
  s21 = _mm_xor_si128(s21, LOAD(in + 144));
  s22 = _mm_xor_si128(s22, LOAD(in + 160));
  s23 = _mm_xor_si128(s23, LOAD(in + 176));
  s30 = _mm_xor_si128(s30, LOAD(in + 192));
  s31 = _mm_xor_si128(s31, LOAD(in + 208));
  s32 = _mm_xor_si128(s32, LOAD(in + 224));
  s33 = _mm_xor_si128(s33, LOAD(in + 240));

  TRUNCSTORE(out, s00, s01, s02, s03);
  TRUNCSTORE(out + 32, s10, s11, s12, s13);
  TRUNCSTORE(out + 64, s20, s21, s22, s23);
  TRUNCSTORE(out + 96, s30, s31, s32, s33);
}

void haraka512_8x(unsigned char *out, const unsigned char *in) {
  // This is faster on Skylake, the code below is faster on Haswell.
  haraka512_4x(out, in);
  haraka512_4x(out + 128, in + 256);

  // u128 s84, tmp;
  //
  // s00 = LOAD(in);
  // s01 = LOAD(in + 16);
  // s02 = LOAD(in + 32);
  // s03 = LOAD(in + 48);
  // s10 = LOAD(in + 64);
  // s11 = LOAD(in + 80);
  // s12 = LOAD(in + 96);
  // s13 = LOAD(in + 112);
  // s20 = LOAD(in + 128);
  // s21 = LOAD(in + 144);
  // s22 = LOAD(in + 160);
  // s23 = LOAD(in + 176);
  // s30 = LOAD(in + 192);
  // s31 = LOAD(in + 208);
  // s32 = LOAD(in + 224);
  // s33 = LOAD(in + 240);
  // s40 = LOAD(in + 256);
  // s41 = LOAD(in + 272);
  // s42 = LOAD(in + 288);
  // s43 = LOAD(in + 304);
  // s50 = LOAD(in + 320);
  // s51 = LOAD(in + 336);
  // s52 = LOAD(in + 352);
  // s53 = LOAD(in + 368);
  // s60 = LOAD(in + 384);
  // s61 = LOAD(in + 400);
  // s62 = LOAD(in + 416);
  // s63 = LOAD(in + 432);
  // s70 = LOAD(in + 448);
  // s71 = LOAD(in + 464);
  // s72 = LOAD(in + 480);
  // s73 = LOAD(in + 496);
  //
  // AES4_8x(s0, s1, s2, s3, s4, s5, s6, s7, 0);
  // MIX4(s00, s01, s02, s03);
  // MIX4(s10, s11, s12, s13);
  // MIX4(s20, s21, s22, s23);
  // MIX4(s30, s31, s32, s33);
  // MIX4(s40, s41, s42, s43);
  // MIX4(s50, s51, s52, s53);
  // MIX4(s60, s61, s62, s63);
  // MIX4(s70, s71, s72, s73);
  //
  // AES4_8x(s0, s1, s2, s3, s4, s5, s6, s7, 8);
  // MIX4(s00, s01, s02, s03);
  // MIX4(s10, s11, s12, s13);
  // MIX4(s20, s21, s22, s23);
  // MIX4(s30, s31, s32, s33);
  // MIX4(s40, s41, s42, s43);
  // MIX4(s50, s51, s52, s53);
  // MIX4(s60, s61, s62, s63);
  // MIX4(s70, s71, s72, s73);
  //
  // AES4_8x(s0, s1, s2, s3, s4, s5, s6, s7, 16);
  // MIX4(s00, s01, s02, s03);
  // MIX4(s10, s11, s12, s13);
  // MIX4(s20, s21, s22, s23);
  // MIX4(s30, s31, s32, s33);
  // MIX4(s40, s41, s42, s43);
  // MIX4(s50, s51, s52, s53);
  // MIX4(s60, s61, s62, s63);
  // MIX4(s70, s71, s72, s73);
  //
  // AES4_8x(s0, s1, s2, s3, s4, s5, s6, s7, 24);
  // MIX4(s00, s01, s02, s03);
  // MIX4(s10, s11, s12, s13);
  // MIX4(s20, s21, s22, s23);
  // MIX4(s30, s31, s32, s33);
  // MIX4(s40, s41, s42, s43);
  // MIX4(s50, s51, s52, s53);
  // MIX4(s60, s61, s62, s63);
  // MIX4(s70, s71, s72, s73);
  //
  // AES4_8x(s0, s1, s2, s3, s4, s5, s6, s7, 32);
  // MIX4(s00, s01, s02, s03);
  // MIX4(s10, s11, s12, s13);
  // MIX4(s20, s21, s22, s23);
  // MIX4(s30, s31, s32, s33);
  // MIX4(s40, s41, s42, s43);
  // MIX4(s50, s51, s52, s53);
  // MIX4(s60, s61, s62, s63);
  // MIX4(s70, s71, s72, s73);
  //
  //
  // s00 = _mm_xor_si128(s00, LOAD(in));
  // s01 = _mm_xor_si128(s01, LOAD(in + 16));
  // s02 = _mm_xor_si128(s02, LOAD(in + 32));
  // s03 = _mm_xor_si128(s03, LOAD(in + 48));
  // s10 = _mm_xor_si128(s10, LOAD(in + 64));
  // s11 = _mm_xor_si128(s11, LOAD(in + 80));
  // s12 = _mm_xor_si128(s12, LOAD(in + 96));
  // s13 = _mm_xor_si128(s13, LOAD(in + 112));
  // s20 = _mm_xor_si128(s20, LOAD(in + 128));
  // s21 = _mm_xor_si128(s21, LOAD(in + 144));
  // s22 = _mm_xor_si128(s22, LOAD(in + 160));
  // s23 = _mm_xor_si128(s23, LOAD(in + 176));
  // s30 = _mm_xor_si128(s30, LOAD(in + 192));
  // s31 = _mm_xor_si128(s31, LOAD(in + 208));
  // s32 = _mm_xor_si128(s32, LOAD(in + 224));
  // s33 = _mm_xor_si128(s33, LOAD(in + 240));
  // s40 = _mm_xor_si128(s40, LOAD(in + 256));
  // s41 = _mm_xor_si128(s41, LOAD(in + 272));
  // s42 = _mm_xor_si128(s42, LOAD(in + 288));
  // s43 = _mm_xor_si128(s43, LOAD(in + 304));
  // s50 = _mm_xor_si128(s50, LOAD(in + 320));
  // s51 = _mm_xor_si128(s51, LOAD(in + 336));
  // s52 = _mm_xor_si128(s52, LOAD(in + 352));
  // s53 = _mm_xor_si128(s53, LOAD(in + 368));
  // s60 = _mm_xor_si128(s60, LOAD(in + 384));
  // s61 = _mm_xor_si128(s61, LOAD(in + 400));
  // s62 = _mm_xor_si128(s62, LOAD(in + 416));
  // s63 = _mm_xor_si128(s63, LOAD(in + 432));
  // s70 = _mm_xor_si128(s70, LOAD(in + 448));
  // s71 = _mm_xor_si128(s71, LOAD(in + 464));
  // s72 = _mm_xor_si128(s72, LOAD(in + 480));
  // s73 = _mm_xor_si128(s73, LOAD(in + 496));
  //
  // TRUNCSTORE(out, s00, s01, s02, s03);
  // TRUNCSTORE(out + 32, s10, s11, s12, s13);
  // TRUNCSTORE(out + 64, s20, s21, s22, s23);
  // TRUNCSTORE(out + 96, s30, s31, s32, s33);
  // TRUNCSTORE(out + 128, s40, s41, s42, s43);
  // TRUNCSTORE(out + 160, s50, s51, s52, s53);
  // TRUNCSTORE(out + 192, s60, s61, s62, s63);
  // TRUNCSTORE(out + 224, s70, s71, s72, s73);
}
