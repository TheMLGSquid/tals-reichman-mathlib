#pragma once

#include <cstdint>

static int memcmp_reverse(const uint8_t* a, const uint8_t* b, size_t size) {
	for (long long i = size - 1; i >= 0; i--) {
		if (a[i] == b[i]) continue;

		return (a[i] > b[i]) ? 1 : -1;
	}
	return 0;
}
