﻿#ifndef lowdiscrepancy_hpp
#define lowdiscrepancy_hpp

#include "header.h"

PALADIN_BEGIN

// 质数表大小
static CONSTEXPR int PrimeTableSize = 1000;
// 质数表
extern const int Primes[PrimeTableSize];
// 质数和，i与v的关系为前i个质数相加，值为v
extern const int PrimeSums[PrimeTableSize];


Float RadicalInverse(int baseIndex, uint64_t a);

Float ScrambledRadicalInverse(int baseIndex, uint64_t a, const uint16_t* perm);

inline uint32_t ReverseBits32(uint32_t n) {
    n = (n << 16) | (n >> 16);
    n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
    n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
    n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
    n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
    return n;
}

inline uint64_t ReverseBits64(uint64_t n) {
    uint64_t n0 = ReverseBits32((uint32_t)n);
    uint64_t n1 = ReverseBits32((uint32_t)(n >> 32));
    return (n0 << 32) | n1;
}

PALADIN_END

#endif /* lowdiscrepancy_hpp */