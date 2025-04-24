#pragma once

#include <stdint.h>

#if (UINTPTR_MAX == UINT32_MAX)
	#define MACHINE_PTR32 1
#elif (UINTPTR_MAX == UINT64_MAX)
	#define MACHINE_PTR64 1
#endif

// Fast log2 (64-bit)

#if _MSC_VER

#include <intrin.h>

	#if MACHINE_PTR64

#pragma intrinsic(_BitScanReverse64)

static uint8_t log2_u64(uint64_t X) {
	unsigned long Result;
	_BitScanReverse64(&Result, X);
	return (uint8_t)Result;
}

	#elif MACHINE_PTR32

#pragma intrinsic(_BitScanReverse)

static uint8_t log2_u64(uint64_t X) {
	unsigned long Result;
	if (_BitScanReverse(&Result, X >> 32))
		return (uint8_t)Result + 32;
	_BitScanReverse(&Result, (uint32_t)X);
	return (uint8_t)Result;
}

	#endif

#elif __GNUC__

static uint8_t log2_u64(uint64_t X) {
	return (uint8_t)(63 - __builtin_clzll(X));
}

#else

// Source: https://www.chessprogramming.org/BitScan#De_Bruijn_Multiplication_2

static const uint8_t aLogTable64[64] = {
	 0, 47,  1, 56, 48, 27,  2, 60,
	57, 49, 41, 37, 28, 16,  3, 61,
	54, 58, 35, 52, 50, 42, 21, 44,
	38, 32, 29, 23, 17, 11,  4, 62,
	46, 55, 26, 59, 40, 36, 15, 53,
	34, 51, 20, 43, 31, 22, 10, 45,
	25, 39, 14, 33, 19, 30,  9, 24,
	13, 18,  8, 12,  7,  6,  5, 63
};

static uint8_t log2_u64(uint64_t X) {
	X |= X >> 1;
	X |= X >> 2;
	X |= X >> 4;
	X |= X >> 8;
	X |= X >> 16;
	X |= X >> 32;
	return aLogTable64[(X * 0x03F79D71B4CB0A89) >> 58];
}

#endif

// Fast log2 (32-bit)

#if _MSC_VER

#pragma intrinsic(_BitScanReverse)

static uint8_t log2_u32(uintptr_t X) {
	unsigned long Result;
	_BitScanReverse(&Result, X);
	return (uint8_t)Result;
}

#elif __GNUC__

static uint8_t log2_u32(uintptr_t X) {
	return (uint8_t)(31 - __builtin_clz(X));
}

#else

// Source: https://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers

static const uint8_t aLogTable32[32] = {
	 0,  9,  1, 10, 13, 21,  2, 29,
	11, 14, 16, 18, 22, 25,  3, 30,
	 8, 12, 20, 28, 15, 17, 24,  7,
	19, 27, 23,  6, 26,  5,  4, 31
};

static uint8_t log2_u32(uintptr_t X) {
	X |= X >> 1;
	X |= X >> 2;
	X |= X >> 4;
	X |= X >> 8;
	X |= X >> 16;
	return aLogTable32[(X * 0x07C4ACDD) >> 27];
}

#endif

// Fast log2 (pointer)

#if MACHINE_PTR64

static uint8_t log2_uptr(uintptr_t X) {
	return log2_u64(X);
}

#elif MACHINE_PTR32

static uint8_t log2_uptr(uintptr_t X) {
	return log2_u32(X);
}

#endif

// Multiply two 64-bit integers to get 128-bit result

static void mul_u64_iso(uint64_t A, uint64_t B, uint64_t (*Result)[2]) {
	uint32_t A0 = (uint32_t)A;
	uint32_t A1 = (uint32_t)(A >> 32);
	uint32_t B0 = (uint32_t)B;
	uint32_t B1 = (uint32_t)(B >> 32);

	uint64_t R00 = (uint64_t)A0 * B0;
	uint64_t R01 = (uint64_t)A0 * B1;
	uint64_t R10 = (uint64_t)A1 * B0;
	uint64_t R11 = (uint64_t)A1 * B1;

	uint64_t Mid = R01 + R10;
	uint8_t Carry = (Mid < R10); // Detect overflow

	(*Result)[0] = R00 + (Mid << 32);
	(*Result)[1] = R11 + (Mid >> 32) + Carry;
}

#if _MSC_VER

	#if MACHINE_PTR64
	
#pragma intrinsic(_umul128)

static void mul_u64(uint64_t A, uint64_t B, uint64_t (*Result)[2]) {
	(*Result)[0] = _umul128(A, B, &(*Result)[1]);
}
		
	#elif MACHINE_PTR32
		
static void mul_u64(uint64_t A, uint64_t B, uint64_t (*Result)[2]) {
	mul_u64_iso(A, B, Result);
}

	#endif

#elif __GNUC__

	#if MACHINE_PTR64
	
#pragma intrinsic(_umul128)

static void mul_u64(uint64_t A, uint64_t B, uint64_t (*Result)[2]) {
	unsigned __int128 Result2 = (unsigned __int128)A * B;
	(*Result)[0] = (uint64_t)Result2;
	(*Result)[1] = (uint64_t)(Result2 >> 64);
}
		
	#elif MACHINE_PTR32
		
static void mul_u64(uint64_t A, uint64_t B, uint64_t (*Result)[2]) {
	mul_u64_iso(A, B, Result);
}

	#endif

#else

static void mul_u64(uint64_t A, uint64_t B, uint64_t (*Result)[2]) {
	mul_u64_iso(A, B, Result);
}
	
#endif
