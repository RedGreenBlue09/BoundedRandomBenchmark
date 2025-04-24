#pragma once

#include <stdint.h>

/* 64-bit RNG */

typedef struct {
	uint64_t s[4];
	// Statistics
	uint64_t CallCount;
} rand64_state;

/* This is xoshiro256** 1.0 */
/* License: http://creativecommons.org/publicdomain/zero/1.0/ */

static inline uint64_t rotl64(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

static uint64_t xoshiro256_next(rand64_state* s) {
	const uint64_t result = rotl64(s->s[1] * 5, 7) * 9;

	const uint64_t t = s->s[1] << 17;

	s->s[2] ^= s->s[0];
	s->s[3] ^= s->s[1];
	s->s[1] ^= s->s[2];
	s->s[0] ^= s->s[3];

	s->s[2] ^= t;

	s->s[3] = rotl64(s->s[3], 45);

	return result;
}

/* This is splitmix64 */

static uint64_t splitmix64_next(uint64_t* state) {
	*state += 0x9e3779b97f4a7c15;
	uint64_t z = *state;
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}

/* Wrappers */

static void srand64(rand64_state* state, uint64_t seed) {
	state->s[0] = splitmix64_next(&seed);
	state->s[1] = splitmix64_next(&seed);
	state->s[2] = splitmix64_next(&seed);
	state->s[3] = splitmix64_next(&seed);
}

static uint64_t rand64(rand64_state* state) {
	state->CallCount += 1;
	return xoshiro256_next(state);
}

static double randf64(rand64_state* state) {
	state->CallCount += 1;
	return (double)(xoshiro256_next(state) >> 11) * 0x1p-53;
}

/* More wrappers */

typedef uint64_t (*rand64_func_t)(rand64_state*);

static uint64_t rand64_slow(rand64_state* state) {
	// Extra instructions to be slow
	volatile uint64_t X = 0xAAAAAAAAAAAAAAAA;
	for (uint16_t i = 0; i < 64; ++i)
		X = ~X;
	return rand64(state);
}

/* 32-bit RNG */

typedef struct {
	uint32_t s[4];
	// Statistics
	uint64_t CallCount;
} rand32_state;

/* This is xoshiro128** 1.1 */
/* License: http://creativecommons.org/publicdomain/zero/1.0/ */

static inline uint32_t rotl32(const uint32_t x, int k) {
	return (x << k) | (x >> (32 - k));
}

uint32_t xoshiro128_next(rand32_state* s) {
	const uint32_t result = rotl32(s->s[1] * 5, 7) * 9;
	
	const uint32_t t = s->s[1] << 9;

	s->s[2] ^= s->s[0];
	s->s[3] ^= s->s[1];
	s->s[1] ^= s->s[2];
	s->s[0] ^= s->s[3];

	s->s[2] ^= t;

	s->s[3] = rotl32(s->s[3], 11);

	return result;
}

/* This is splitmix32 */

static uint32_t splitmix32_next(uint32_t* state) {
	*state += 0x9e3779b9;
	uint32_t z = *state;
	z = (z ^ (z >> 16)) * 0x21f0aaad;
	z = (z ^ (z >> 15)) * 0x735a2d97;
	return z ^ (z >> 15);
}

/* Wrappers */

static void srand32(rand32_state* state, uint32_t seed) {
	state->s[0] = splitmix32_next(&seed);
	state->s[1] = splitmix32_next(&seed);
	state->s[2] = splitmix32_next(&seed);
	state->s[3] = splitmix32_next(&seed);
}

static void srand32_64(rand32_state* state, uint64_t seed) {
	uint32_t seed0 = (uint32_t)seed;
	uint32_t seed1 = (uint32_t)(seed >> 32);
	state->s[0] = splitmix32_next(&seed0);
	state->s[1] = splitmix32_next(&seed0);
	state->s[2] = splitmix32_next(&seed1);
	state->s[3] = splitmix32_next(&seed1);
}

static uint32_t rand32(rand32_state* state) {
	state->CallCount += 1;
	return xoshiro128_next(state);
}

static float randf32(rand32_state* state) {
	state->CallCount += 1;
	return (float)(xoshiro128_next(state) >> 8) * 0x1p-24f;
}

/* More wrappers */

typedef uint32_t (*rand32_func_t)(rand32_state*);

static uint32_t rand32_slow(rand32_state* state) {
	// Extra instructions to be slow
	volatile uint32_t X = 0xAAAAAAAA;
	for (uint16_t i = 0; i < 64; ++i)
		X = ~X;
	return rand32(state);
}
