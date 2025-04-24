#pragma once

#include "IntMath.h"
#include "Random.h"

static uint32_t rand32_bounded_bitmask(rand32_func_t rand32_function, rand32_state* state, uint32_t max_value) {
	uint32_t mask = UINT32_MAX >> (31 - log2_u32(max_value | 1));
	uint32_t x;
	do {
		x = rand32_function(state) & mask;
	} while (x > max_value);
	return x;
}

static uint32_t rand32_bounded_short_product(rand32_func_t rand32_function, rand32_state* state, uint32_t max_value) {
	if (max_value == UINT32_MAX)
		return rand32_function(state);

	uint32_t upper = max_value + 1;
	const uint8_t max_followup_iterations = 10;

	uint32_t rand = rand32_function(state);
	uint64_t prod;
	prod = (uint64_t)upper * rand;
	uint32_t i = (uint32_t)(prod >> 32);
	uint32_t f = (uint32_t)prod;
	if (f <= 0 - upper)
		return i;

	for (uint8_t j = 0; j < max_followup_iterations; ++j) {
		rand = rand32_function(state);
		prod = (uint64_t)upper * rand;
		uint32_t f2 = (uint32_t)(prod >> 32);
		f += f2;
		
		if (f < f2)
			return i + 1;

		if (f != UINT32_MAX)
			return i;

		f = (uint32_t)prod;
	}
	return i;
}

static uint32_t rand32_bounded_multiply(rand32_func_t rand32_function, rand32_state* state, uint32_t max_value) {
	if (max_value == UINT32_MAX)
		return rand32_function(state);

	uint32_t range = max_value + 1;
	uint32_t t = (0 - range) % range;
	uint64_t m;
	do {
		m = (uint64_t)rand32_function(state) * range;
	} while ((uint32_t)m < t);
	return (uint32_t)(m >> 32);
}

static uint32_t rand32_bounded_multiply_2(rand32_func_t rand32_function, rand32_state* state, uint32_t max_value) {
	if (max_value == UINT32_MAX)
		return rand32_function(state);

	uint32_t range = max_value + 1;
	uint64_t m;
	m = (uint64_t)rand32_function(state) * range;
	if ((uint32_t)m < range) {
		uint32_t t = 0 - range;
		if (t >= range) {
			t -= range;
			if (t >= range)
				t %= range;
		}
		while ((uint32_t)m < t)
			m = (uint64_t)rand32_function(state) * range;
	}
	return (uint32_t)(m >> 32);
}

static uint32_t rand32_bounded_modulo(rand32_func_t rand32_function, rand32_state* state, uint32_t max_value) {
	if (max_value == UINT32_MAX)
		return rand32_function(state);

	uint32_t range = max_value + 1;
	uint32_t x, r;
	do {
		x = rand32_function(state);
		r = x % range;
	} while (x - r > (0 - range));
	return r;
}

static uint32_t rand32_bounded_modulo_2(rand32_func_t rand32_function, rand32_state* state, uint32_t max_value) {
	if (max_value == UINT32_MAX)
		return rand32_function(state);

	uint32_t range = max_value + 1;
	uint32_t r = rand32_function(state);
	if (r < range) {
		uint32_t t = 0 - range;
		if (t >= range) {
			t -= range;
			if (t >= range)
				t %= range;
		}
		while (r < t)
			r = rand32_function(state);
	}
	if (r >= range) {
		r -= range;
		if (r >= range)
			r %= range;
	}
	return r;
}
