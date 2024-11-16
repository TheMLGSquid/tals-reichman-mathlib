#include "BigNumberBuffer.h"
#include <bit>
#include <cassert>
#include <memory>
#include "number_utils.h"


BigNumberBuffer::BigNumberBuffer(uint8_t len_exponent)
	: len_exponent_(len_exponent), data_(len()) {

}

BigNumberBuffer BigNumberBuffer::from_number(uint32_t num) {
	BigNumberBuffer buffer(2); // 4 bytes

	assert(sizeof(num) == buffer.len());
	memcpy(buffer.data().data(), &num, sizeof(num));
	return buffer;
}
BigNumberBuffer BigNumberBuffer::from_number(uint8_t num) {
	BigNumberBuffer buffer(0); // 1 byte

	assert(sizeof(num) == buffer.len());
	memcpy(buffer.data().data(), &num, sizeof(num));
	return buffer;
}
BigNumberBuffer BigNumberBuffer::from_number(uint16_t num) {
	BigNumberBuffer buffer(1); // 2 bytes

	assert(sizeof(num) == buffer.len());
	memcpy(buffer.data().data(), &num, sizeof(num));
	return buffer;
}
BigNumberBuffer BigNumberBuffer::of_size(size_t power_of_two) {
	return BigNumberBuffer(ilog2(power_of_two));
}
size_t BigNumberBuffer::len() const {
	return 1ull << len_exponent_;
}

uint8_t BigNumberBuffer::exponent() const {
	return len_exponent_;
}

std::span<uint8_t> BigNumberBuffer::data() {
	return std::span<uint8_t>(data_.begin(), data_.end());
}
std::span<const uint8_t> BigNumberBuffer::data() const {
	return std::span<const uint8_t>(data_.begin(), data_.end());
}

BigNumberBuffer BigNumberBuffer::extended(uint8_t new_len_exponent) const {
	if (new_len_exponent == len_exponent_) {
		return *this;
	}

	assert(new_len_exponent >= len_exponent_);

	BigNumberBuffer result(new_len_exponent);

	memcpy(result.data_.data(), data_.data(), len());

	return result;
}

void BigNumberBuffer::extend(uint8_t new_len_exponent) {
	assert(new_len_exponent >= len_exponent_);

	data_.resize(len_from_exponent(new_len_exponent), 0);
}
void BigNumberBuffer::extend() {
	data_.resize(len_from_exponent(++len_exponent_), 0);
}

size_t BigNumberBuffer::len_from_exponent(uint8_t exponent) {
	return 1ull << exponent;
}