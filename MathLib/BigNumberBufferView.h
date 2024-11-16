#pragma once
#include "BigNumberBuffer.h"

#include <memory>
#include <span>

class BigNumberBufferView {
public:
	BigNumberBufferView(BigNumberBuffer&& buffer);
	BigNumberBufferView(std::shared_ptr<const BigNumberBuffer> buffer);
	BigNumberBufferView(std::shared_ptr<const BigNumberBuffer> buffer,
		size_t offset, uint8_t exponent);
	
	inline size_t len() const;
	uint8_t exponent() const;

	std::span<const uint8_t> data() const;

	BigNumberBufferView high() const;
	BigNumberBufferView low() const;

	BigNumberBufferView& operator = (const BigNumberBufferView& other);

	const BigNumberBuffer& buffer() const;

	bool should_extend(size_t count) const;

	BigNumberBufferView shift_left(size_t count) const;
	BigNumberBufferView shift_right(size_t count) const;

	BigNumberBufferView extended(uint8_t new_len_exponent) const;
private:
	size_t offset_;
	uint8_t exponent_;
	std::shared_ptr<const BigNumberBuffer> buffer_;
};
