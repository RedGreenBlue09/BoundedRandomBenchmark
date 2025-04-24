#pragma once

#include <stdint.h>

#if _WIN32

#include <Windows.h>

static uint64_t ClockRes = 0;

static uint64_t clock64() {
	LARGE_INTEGER TimeStruct;
	QueryPerformanceCounter(&TimeStruct);
	return TimeStruct.QuadPart;
}

static uint64_t clock64_resolution() {
	if (ClockRes == 0) {
		LARGE_INTEGER TimeStruct;
		QueryPerformanceFrequency(&TimeStruct);
		ClockRes = TimeStruct.QuadPart;
	}
	return ClockRes;
}

// No POSIX implementation yet.

#endif
