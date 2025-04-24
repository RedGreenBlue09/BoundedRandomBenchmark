
#include <inttypes.h>
#include <stdio.h>

#include "BoundedRandom64.h"
#include "BoundedRandom32.h"
#include "Time.h"

typedef struct {
	uint64_t (*Function)(rand64_func_t, rand64_state*, uint64_t);
	const char* sName;
} bounded_rand64_info_t;

const bounded_rand64_info_t gaBoundedRand64Info[] = {
	{rand64_bounded_bitmask,       "Bitmask"      },
	{rand64_bounded_short_product, "Short product"},
	{rand64_bounded_multiply,      "Multiply"     },
	{rand64_bounded_multiply_2,    "Multiply 2"   },
	{rand64_bounded_modulo,        "Modulo"       },
	{rand64_bounded_modulo_2,      "Modulo 2"     },
};

const size_t gnBoundedRand64Info = sizeof(gaBoundedRand64Info) / sizeof(*gaBoundedRand64Info);

typedef struct {
	uint32_t (*Function)(rand32_func_t, rand32_state*, uint32_t);
	const char* sName;
} bounded_rand32_info_t;

const bounded_rand32_info_t gaBoundedRand32Info[] = {
	{rand32_bounded_bitmask,       "Bitmask"      },
	{rand32_bounded_short_product, "Short product"},
	{rand32_bounded_multiply,      "Multiply"     },
	{rand32_bounded_multiply_2,    "Multiply 2"   },
	{rand32_bounded_modulo,        "Modulo"       },
	{rand32_bounded_modulo_2,      "Modulo 2"     },
};

const size_t gnBoundedRand32Info = sizeof(gaBoundedRand32Info) / sizeof(*gaBoundedRand32Info);

int main() {
	const uint64_t Microsecond = clock64_resolution() / 1000000;
	const uint64_t TrialCount = 100000000;
	uint64_t TimeStart;
	uint64_t TimeEnd;
	
	// 64-bit RNG
	rand64_state Rng64State;
	rand64_state Rng64State2;
	srand64(&Rng64State, clock64()); // 64-bit seed is more than enough.
	srand64(&Rng64State2, clock64() + 1);
	
	printf("\n64-bit RNG\n\n");
	
	for (size_t i = 0; i < gnBoundedRand64Info; ++i) {
		bounded_rand64_info_t BoundedRand64Info = gaBoundedRand64Info[i];
		Rng64State.CallCount = 0;
		
		TimeStart = clock64();
		for (uint64_t ii = 0; ii < TrialCount; ++ii) {
			volatile uint64_t Result = BoundedRand64Info.Function(rand64, &Rng64State, rand64(&Rng64State2));
		}
		TimeEnd = clock64();
		
		printf("Large range + fast RNG + %s\n", BoundedRand64Info.sName);
		printf("Time: %"PRIu64" us\n", (TimeEnd - TimeStart) / Microsecond);
		printf("Rng calls: %"PRIu64"\n\n", Rng64State.CallCount);
	}
	
	for (size_t i = 0; i < gnBoundedRand64Info; ++i) {
		bounded_rand64_info_t BoundedRand64Info = gaBoundedRand64Info[i];
		Rng64State.CallCount = 0;
		
		TimeStart = clock64();
		for (uint64_t ii = 0; ii < TrialCount; ++ii) {
			volatile uint64_t Result = BoundedRand64Info.Function(rand64_slow, &Rng64State, rand64(&Rng64State2));
		}
		TimeEnd = clock64();
		
		printf("Large range + slow RNG + %s\n", BoundedRand64Info.sName);
		printf("Time: %"PRIu64" us\n", (TimeEnd - TimeStart) / Microsecond);
		printf("Rng calls: %"PRIu64"\n\n", Rng64State.CallCount);
	}
	
	for (size_t i = 0; i < gnBoundedRand64Info; ++i) {
		bounded_rand64_info_t BoundedRand64Info = gaBoundedRand64Info[i];
		Rng64State.CallCount = 0;
		
		TimeStart = clock64();
		for (uint64_t ii = 0; ii < TrialCount; ++ii) {
			volatile uint64_t Result = BoundedRand64Info.Function(rand64, &Rng64State, rand64(&Rng64State2) & 1023);
		}
		TimeEnd = clock64();
		
		printf("Small range + fast RNG + %s\n", BoundedRand64Info.sName);
		printf("Time: %"PRIu64" us\n", (TimeEnd - TimeStart) / Microsecond);
		printf("Rng calls: %"PRIu64"\n\n", Rng64State.CallCount);
	}
	
	for (size_t i = 0; i < gnBoundedRand64Info; ++i) {
		bounded_rand64_info_t BoundedRand64Info = gaBoundedRand64Info[i];
		Rng64State.CallCount = 0;
		
		TimeStart = clock64();
		for (uint64_t ii = 0; ii < TrialCount; ++ii) {
			volatile uint64_t Result = BoundedRand64Info.Function(rand64_slow, &Rng64State, rand64(&Rng64State2) & 1023);
		}
		TimeEnd = clock64();
		
		printf("Small range + slow RNG + %s\n", BoundedRand64Info.sName);
		printf("Time: %"PRIu64" us\n", (TimeEnd - TimeStart) / Microsecond);
		printf("Rng calls: %"PRIu64"\n\n", Rng64State.CallCount);
	}
	
	// 32-bit RNG
	
	rand32_state Rng32State;
	rand32_state Rng32State2;
	srand32_64(&Rng32State, clock64()); // 64-bit seed is more than enough.
	srand32_64(&Rng32State2, clock64() + 1);
	
	printf("\n32-bit RNG\n\n");
	
	for (size_t i = 0; i < gnBoundedRand32Info; ++i) {
		bounded_rand32_info_t BoundedRand32Info = gaBoundedRand32Info[i];
		Rng32State.CallCount = 0;
		
		TimeStart = clock64();
		for (uint64_t ii = 0; ii < TrialCount; ++ii) {
			volatile uint32_t Result = BoundedRand32Info.Function(rand32, &Rng32State, rand32(&Rng32State2));
		}
		TimeEnd = clock64();
		
		printf("Large range + fast RNG + %s\n", BoundedRand32Info.sName);
		printf("Time: %"PRIu64" us\n", (TimeEnd - TimeStart) / Microsecond);
		printf("Rng calls: %"PRIu64"\n\n", Rng32State.CallCount);
	}
	
	for (size_t i = 0; i < gnBoundedRand32Info; ++i) {
		bounded_rand32_info_t BoundedRand32Info = gaBoundedRand32Info[i];
		Rng32State.CallCount = 0;
		
		TimeStart = clock64();
		for (uint64_t ii = 0; ii < TrialCount; ++ii) {
			volatile uint32_t Result = BoundedRand32Info.Function(rand32_slow, &Rng32State, rand32(&Rng32State2));
		}
		TimeEnd = clock64();
		
		printf("Large range + slow RNG + %s\n", BoundedRand32Info.sName);
		printf("Time: %"PRIu64" us\n", (TimeEnd - TimeStart) / Microsecond);
		printf("Rng calls: %"PRIu64"\n\n", Rng32State.CallCount);
	}
	
	for (size_t i = 0; i < gnBoundedRand32Info; ++i) {
		bounded_rand32_info_t BoundedRand32Info = gaBoundedRand32Info[i];
		Rng32State.CallCount = 0;
		
		TimeStart = clock64();
		for (uint64_t ii = 0; ii < TrialCount; ++ii) {
			volatile uint32_t Result = BoundedRand32Info.Function(rand32, &Rng32State, rand32(&Rng32State2) & 1023);
		}
		TimeEnd = clock64();
		
		printf("Small range + fast RNG + %s\n", BoundedRand32Info.sName);
		printf("Time: %"PRIu64" us\n", (TimeEnd - TimeStart) / Microsecond);
		printf("Rng calls: %"PRIu64"\n\n", Rng32State.CallCount);
	}
	
	for (size_t i = 0; i < gnBoundedRand32Info; ++i) {
		bounded_rand32_info_t BoundedRand32Info = gaBoundedRand32Info[i];
		Rng32State.CallCount = 0;
		
		TimeStart = clock64();
		for (uint64_t ii = 0; ii < TrialCount; ++ii) {
			volatile uint32_t Result = BoundedRand32Info.Function(rand32_slow, &Rng32State, rand32(&Rng32State2) & 1023);
		}
		TimeEnd = clock64();
		
		printf("Small range + slow RNG + %s\n", BoundedRand32Info.sName);
		printf("Time: %"PRIu64" us\n", (TimeEnd - TimeStart) / Microsecond);
		printf("Rng calls: %"PRIu64"\n\n", Rng32State.CallCount);
	}
	
	return 0;
}

