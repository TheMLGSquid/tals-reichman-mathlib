#include "BigNumberBufferView.h"
#include "number_utils.h"
#include <cassert>

BigNumberBufferView::BigNumberBufferView(BigNumberBuffer&& buffer)
	: offset_(0), exponent_(buffer.exponent()), buffer_(std::make_shared<BigNumberBuffer>(buffer)) {}

BigNumberBufferView::BigNumberBufferView(std::shared_ptr<const BigNumberBuffer> buffer)
	: offset_(0), exponent_(buffer->exponent()), buffer_(buffer) {}

BigNumberBufferView::BigNumberBufferView(std::shared_ptr<const BigNumberBuffer> buffer, size_t offset, uint8_t exponent)
	: offset_(offset), exponent_(exponent), buffer_(buffer) {
	assert(exponent <= buffer->exponent());
}

inline size_t BigNumberBufferView::len() const {
	return BigNumberBuffer::len_from_exponent(exponent_);
}

uint8_t BigNumberBufferView::exponent() const {
	return exponent_;
}

std::span<const uint8_t> BigNumberBufferView::data() const {
	return buffer_->data().subspan(offset_, len());
}

BigNumberBufferView BigNumberBufferView::high() const {
	return BigNumberBufferView(buffer_, offset_ + (len() >> 1), exponent_ - 1);
}

BigNumberBufferView BigNumberBufferView::low() const {
	return BigNumberBufferView(buffer_, offset_, exponent_ - 1);
}

BigNumberBufferView& BigNumberBufferView::operator=(const BigNumberBufferView& other) {
	offset_ = other.offset_;
	exponent_ = other.exponent_;
	buffer_ = other.buffer_;

	return *this;
}

const BigNumberBuffer& BigNumberBufferView::buffer() const {
	return *buffer_;
}

bool BigNumberBufferView::should_extend(size_t count) const {
	uint8_t offset_within_byte = count & 0b111;
	for (int i = 0; i < buffer_->len(); i++) {
		if ((count >> 3) + i >= len()
				&& data()[i] << offset_within_byte > 0) {
			return true;
		}
	}
	return false;
}

BigNumberBufferView BigNumberBufferView::shift_left(size_t count) const {
	BigNumberBuffer buffer = should_extend(count)
		? BigNumberBuffer::of_size(upper_power_of_two(len() + count / 8 + 1))
		: BigNumberBuffer(exponent());

	uint8_t offset_within_byte = count & 0b111;

	for (int i = 0; i < len(); i++) {
		uint8_t value = data()[i] << offset_within_byte;
		if (!value) continue;
		buffer.data()[count / 8 + i] = value;
	}

	return BigNumberBufferView(
		std::make_shared<BigNumberBuffer>(BigNumberBuffer(buffer)));
}

BigNumberBufferView BigNumberBufferView::shift_right(size_t count) const {
	size_t new_length = len() - count / 8;
	BigNumberBuffer buffer(ilog2(upper_power_of_two(new_length)));
	
	uint8_t offset_within_byte = count & 0b111;

	for (int i = 0; i < new_length; i++) {
		uint8_t value = data()[count / 8 + i] >> offset_within_byte;
		if (!value) continue;
		buffer.data()[i] = value;
	}

	return BigNumberBufferView(
		std::make_shared<BigNumberBuffer>(BigNumberBuffer(buffer)));
}

BigNumberBufferView BigNumberBufferView::extended(uint8_t new_len_exponent) const {
	if (new_len_exponent == exponent()) {
		return *this;
	}

	return BigNumberBufferView(buffer().extended(new_len_exponent));
}
