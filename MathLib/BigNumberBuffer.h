#pragma once

#include <stdint.h>
#include <vector>
#include <span>


class BigNumberBuffer {
public:
	explicit BigNumberBuffer(uint8_t len_exponent);

	static BigNumberBuffer from_number(uint32_t num);
	static BigNumberBuffer from_number(uint8_t num);
	static BigNumberBuffer from_number(uint16_t num);
	static BigNumberBuffer of_size(size_t power_of_two);

	inline size_t len() const;
	uint8_t exponent() const;
	
	std::span<uint8_t> data();
	std::span<const uint8_t> data() const;

	BigNumberBuffer extended(uint8_t new_len_exponent) const;

	void extend(uint8_t new_len_exponent);
	void extend();
	static size_t len_from_exponent(uint8_t exponent);
private:
	uint8_t len_exponent_; // 8 bits will suffice
	std::vector<uint8_t> data_;
};

