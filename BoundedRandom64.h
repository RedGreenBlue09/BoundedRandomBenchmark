#pragma once

#include "IntMath.h"
#include "Random.h"

static uint64_t rand64_bounded_bitmask(rand64_func_t rand64_function, rand64_state* state, uint64_t max_value) {
	uint64_t mask = UINT64_MAX >> (63 - log2_u64(max_value | 1));
	uint64_t x;
	do {
		x = rand64_function(state) & mask;
	} while (x > max_value);
	return x;
}

static uint64_t rand64_bounded_short_product(rand64_func_t rand64_function, rand64_state* state, uint64_t max_value) {
	if (max_value == UINT64_MAX)
		return rand64_function(state);

	uint64_t upper = max_value + 1;
	const uint8_t max_followup_iterations = 10;

	uint64_t rand = rand64_function(state);
	uint64_t prod[2];
	mul_u64(upper, rand, &prod);
	uint64_t i = prod[1];
	uint64_t f = prod[0];
	if (f <= 0 - upper)
		return i;

	for (uint8_t j = 0; j < max_followup_iterations; ++j) {
		rand = rand64_function(state);
		mul_u64(upper, rand, &prod);
		uint64_t f2 = prod[1];
		f += f2;
		
		if (f < f2)
			return i + 1;

		if (f != UINT64_MAX)
			return i;

		f = prod[0];
	}
	return i;
}

static uint64_t rand64_bounded_multiply(rand64_func_t rand64_function, rand64_state* state, uint64_t max_value) {
	if (max_value == UINT64_MAX)
		return rand64_function(state);

	uint64_t range = max_value + 1;
	uint64_t t = (0 - range) % range;
	uint64_t m[2];
	do {
		mul_u64(rand64_function(state), range, &m);
	} while (m[0] < t);
	return m[1];
}

static uint64_t rand64_bounded_multiply_2(rand64_func_t rand64_function, rand64_state* state, uint64_t max_value) {
	if (max_value == UINT64_MAX)
		return rand64_function(state);

	uint64_t range = max_value + 1;
	uint64_t m[2];
	mul_u64(rand64_function(state), range, &m);
	if (m[0] < range) {
		uint64_t t = 0 - range;
		if (t >= range) {
			t -= range;
			if (t >= range)
				t %= range;
		}
		while (m[0] < t)
			mul_u64(rand64_function(state), range, &m);
	}
	return m[1];
}

static uint64_t rand64_bounded_modulo(rand64_func_t rand64_function, rand64_state* state, uint64_t max_value) {
	if (max_value == UINT64_MAX)
		return rand64_function(state);

	uint64_t range = max_value + 1;
	uint64_t x, r;
	do {
		x = rand64_function(state);
		r = x % range;
	} while (x - r > (0 - range));
	return r;
}

static uint64_t rand64_bounded_modulo_2(rand64_func_t rand64_function, rand64_state* state, uint64_t max_value) {
	if (max_value == UINT64_MAX)
		return rand64_function(state);

	uint64_t range = max_value + 1;
	uint64_t r = rand64_function(state);
	if (r < range) {
		uint64_t t = 0 - range;
		if (t >= range) {
			t -= range;
			if (t >= range)
				t %= range;
		}
		while (r < t)
			r = rand64_function(state);
	}
	if (r >= range) {
		r -= range;
		if (r >= range)
			r %= range;
	}
	return r;
}
