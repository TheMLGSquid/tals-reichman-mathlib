#pragma once
#include <bit>

static constexpr size_t ilog2(uint32_t x)
{
	return sizeof(uint32_t) * CHAR_BIT - std::countl_zero(x) - 1;
}

static size_t upper_power_of_two(size_t v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}