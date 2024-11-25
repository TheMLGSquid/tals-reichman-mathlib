#pragma once
#include <iomanip>
#include "BigNumberBufferView.h"
#include "BigNumberBuffer.h"
#include "iter_utils.h"
#include <cmath>
#include <cstring>
#include <iostream>

class BigNumber {
public:
	const static BigNumber ONE;
	const static BigNumber ZERO;

	BigNumber();

	BigNumber(uint32_t number);
	BigNumber(uint8_t number);
	BigNumber(uint16_t number);

	static const BigNumber power_of_two(size_t exponent);

	bool sign() const { return sign_; }
	bool non_negative() const { return !sign_ || is_zero(); }
	bool is_zero() const;
	BigNumber negated() const;
	void negate();

	BigNumber with_extended_buffer(uint8_t exponent) const;

	int compare_absolute_value(const BigNumber& other) const;

	BigNumber minus(const BigNumber& other) const;

	BigNumber add(const BigNumber& other) const;

	BigNumber shift_left(size_t count) const;
	BigNumber shift_right(size_t count) const;

	BigNumber mult(const BigNumber& other) const;
	BigNumber mod(const BigNumber& other) const;
	BigNumber div(const BigNumber& other, BigNumber *remainder) const;

	BigNumber div_different_signs(const BigNumber& other, BigNumber* remainder) const;

	BigNumber& operator = (const BigNumber& other);
	BigNumber operator + (const BigNumber& other) const;
	BigNumber operator - (const BigNumber& other) const;
	BigNumber operator * (const BigNumber& other) const;
	BigNumber operator / (const BigNumber& other) const;
	BigNumber operator << (size_t count) const;
	bool operator < (const BigNumber& rhs) const;
	bool operator == (const BigNumber& rhs) const;
	bool operator <= (const BigNumber& rhs) const;

	BigNumber high() const;
	BigNumber low() const;

	void print(std::ostream& stream) const;
	inline friend static std::ostream& operator<< (std::ostream& stream, const BigNumber& number) {
		number.print(stream);
		return stream;
	}

	explicit BigNumber(const BigNumberBufferView& buffer, bool sign = false);
	explicit BigNumber(BigNumberBufferView&& buffer, bool sign = false);
private:
	BigNumberBufferView buffer_view_;
	bool sign_; // Unused for now
};
