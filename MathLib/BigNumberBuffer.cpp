#include "BigNumberBuffer.h"
#include <bit>
#include <cassert>
#include <memory>
#include "number_utils.h"
#include <iostream>

BigNumberBuffer::BigNumberBuffer(uint8_t len_exponent)
	: len_exponent_(len_exponent), data_() {
	if (len_exponent <= 3) {
		data_.emplace<long long>();
		return;
	}
	data_.emplace<std::vector<uint8_t>>(len());
}
BigNumberBuffer::~BigNumberBuffer() {
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
	if (data_.index() == 0) {
		auto&& vec = std::get<0>(data_);
		return std::span<uint8_t>(vec.begin(), len());
	}

	auto& num = std::get<1>(data_);
	return std::span<uint8_t>(
		reinterpret_cast<uint8_t*>(&num), len());
}
std::span<const uint8_t> BigNumberBuffer::data() const {
	if (data_.index() == 0) {
		auto&& vec = std::get<0>(data_);
		return std::span<const uint8_t>(vec.begin(), len());
	}

	auto& num = std::get<1>(data_);
	return std::span<const uint8_t>(
		reinterpret_cast<const uint8_t*>(&num), len());

}

BigNumberBuffer BigNumberBuffer::extended(uint8_t new_len_exponent) const {
	if (new_len_exponent == len_exponent_) {
		return *this;
	}

	assert(new_len_exponent >= len_exponent_);

	BigNumberBuffer result(new_len_exponent);

	memcpy(result.data().data(), data().data(), len());

	return result;
}

void BigNumberBuffer::extend(uint8_t new_len_exponent) {
	assert(new_len_exponent >= len_exponent_);
	size_t new_len = len_from_exponent(new_len_exponent);

	if (data_.index() == 0) {
		std::get<0>(data_).resize(new_len, 0);
		return;
	}

	if (new_len_exponent <= 3) return;
	extend_from_inline_to_buf(new_len_exponent);
}

void BigNumberBuffer::extend_from_inline_to_buf(uint8_t new_len_exponent)
{
	long long val = std::get<1>(data_);
	data_.emplace<std::vector<uint8_t>>(new_len_exponent);
	memcpy(std::get<0>(data_).data(), &val, sizeof(val));
}

void BigNumberBuffer::extend() {
	uint8_t new_len_exponent = len_from_exponent(++len_exponent_);

	if (data_.index() == 0) {
		std::get<0>(data_).resize(new_len_exponent, 0);
		return;
	}

	if (new_len_exponent <= 3) return;
	extend_from_inline_to_buf(new_len_exponent);
}

size_t BigNumberBuffer::len_from_exponent(uint8_t exponent) {
	return 1ull << exponent;
}