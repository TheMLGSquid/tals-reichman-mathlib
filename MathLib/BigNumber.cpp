#include "BigNumber.h"
#include "number_utils.h"

inline static uint8_t op_with_overflow(bool addition, uint8_t a, uint8_t b, uint8_t c, bool* overflow) {
	int16_t result;

	if (addition) {
		result = (static_cast<int16_t>(a) + b) + c;
		*overflow = result > std::numeric_limits<uint8_t>::max();
	}
	else {
		result = (static_cast<int16_t>(a) - b) - c;
		*overflow = result < 0;
	}

	return static_cast<uint8_t>(result);
}

BigNumber::BigNumber() : buffer_view_(BigNumberBufferView(
	std::make_shared<BigNumberBuffer>(2))), sign_(false) { }

BigNumber::BigNumber(uint32_t number)
	: buffer_view_(BigNumberBufferView(
		std::make_shared<BigNumberBuffer>(
			BigNumberBuffer::from_number(number)))),
	sign_(false) {
}

BigNumber::BigNumber(uint8_t number)
	: buffer_view_(BigNumberBufferView(
		std::make_shared<BigNumberBuffer>(
			BigNumberBuffer::from_number(number)))),
	sign_(false) {
}

BigNumber::BigNumber(uint16_t number)
	: buffer_view_(BigNumberBufferView(
		std::make_shared<BigNumberBuffer>(
			BigNumberBuffer::from_number(number)))),
	sign_(false) {
}

const BigNumber BigNumber::power_of_two(size_t exponent) {
	return BigNumber(BigNumberBufferView(
		BigNumberBuffer(static_cast<uint8_t>(
			upper_power_of_two((exponent >> 3) + 1)))));
}

BigNumber BigNumber::negated() const {
	BigNumber copy = *this;
	copy.negate();
	return copy;
}

void BigNumber::negate() {
	sign_ = !sign_;
}

BigNumber BigNumber::with_extended_buffer(uint8_t exponent) const {
	if (buffer_view_.exponent() == exponent) {
		return *this;
	}

	return BigNumber(buffer_view_.extended(exponent), sign_);
}

int BigNumber::compare_absolute_value(const BigNumber& other) const {
	return memcmp_reverse(buffer_view_.data().data(), other.buffer_view_.data().data(),
		buffer_view_.len());
}

BigNumber BigNumber::minus(const BigNumber& other) const {
	return add(other.negated());
}

BigNumber BigNumber::add(const BigNumber& rhs) const {
	// Sanitize: rhs >= 0
	if (sign_) {
		return negated().minus(rhs).negated();
	}

	// Sanitize: |rhs| > |lhs|
	if (buffer_view_.len() < rhs.buffer_view_.len()) {
		return rhs.add(*this);
	}

	// Sanitize: abs(rhs) > abs(lhs)
	if (sign_ != rhs.sign_ && compare_absolute_value(rhs) == -1) {
		return rhs
			.with_extended_buffer(buffer_view_.exponent())
			.negated()
			.minus(*this)
			.negated();
	}

	bool addition = !rhs.sign_;
	size_t rhs_len = rhs.buffer_view_.len();
	size_t lhs_len = buffer_view_.len();

	BigNumberBuffer result_buffer(buffer_view_.exponent());
	std::span<const uint8_t> lhs_data = buffer_view_.data(),
		rhs_data = rhs.buffer_view_.data();

	bool carry_or_borrow = false;
	for (size_t i = 0; i < rhs_len; i++) {
		uint8_t a = lhs_data[i], b = rhs_data[i];

		result_buffer.data()[i] = op_with_overflow(addition, a, b,
			static_cast<uint8_t>(carry_or_borrow), &carry_or_borrow);
	}

	for (size_t i = rhs_len; i < lhs_len; i++) {
		result_buffer.data()[i] = op_with_overflow(addition,
			lhs_data[i], 0,
			static_cast<uint8_t>(carry_or_borrow), &carry_or_borrow);
	}

	if (carry_or_borrow) {
		result_buffer.extend();
		result_buffer.data()[result_buffer.len() >> 1] = 1;
	}

	return BigNumber(BigNumberBufferView(
		std::make_shared<BigNumberBuffer>(result_buffer)),
		false);
}

BigNumber BigNumber::shift_left(size_t count) const {
	return BigNumber(buffer_view_.shift_left(count), sign_);
}
BigNumber BigNumber::shift_right(size_t count) const {
	return BigNumber(buffer_view_.shift_right(count), sign_);
}

BigNumber BigNumber::mult(const BigNumber& rhs) const {
	// Sanitize: rhs.exponent >= lhs.exponent
	if (buffer_view_.exponent() < rhs.buffer_view_.exponent()) {
		return rhs.mult(*this);
	}

	// Sanitize: rhs.exponent == lhs.exponent 
	if (buffer_view_.exponent() != rhs.buffer_view_.exponent()) {
		return mult(BigNumber(
			rhs.buffer_view_.extended(buffer_view_.exponent())));
	}

	// Base case - both buffers are a single 8-bit digit.
	if (buffer_view_.exponent() == 0) {
		uint16_t result = static_cast<uint16_t>(buffer_view_.data()[0])
			* rhs.buffer_view_.data()[0];

		return BigNumber(result);
	}

	BigNumber a = high(), b = low(),
		c = rhs.high(), d = rhs.low();

	BigNumber A1 = a * c;
	BigNumber A2 = b * d;
	BigNumber A3 = (a + b) * (c + d);

	size_t len = buffer_view_.len();

	BigNumber result = (A1 << (len << 3)) // A1 * 2^(len * 8)
		+ ((A3 - A1 - A2) << (len << 2)) // (A3 - A1 - A2) * 2^(len*4)
		+ A2;

	return sign_ != rhs.sign_
		? result.negated() : result;
}

BigNumber& BigNumber::operator=(const BigNumber& other) {
	buffer_view_ = other.buffer_view_;
	sign_ = other.sign_;

	return *this;
}

BigNumber BigNumber::high() const {
	return BigNumber(buffer_view_.high());
}

BigNumber BigNumber::low() const {
	return BigNumber(buffer_view_.low());
}

void BigNumber::print(std::ostream& stream) const {
	if (sign_) {
		stream << "Negative ";
	}
	std::span<const uint8_t> data = buffer_view_.data();
	for (long long i = data.size() - 1; i >= 0; i--) {
		stream << std::hex << std::setw(2) << std::setfill('0')
			<< (int)data[i] << " ";
	}
}

BigNumber::BigNumber(const BigNumberBufferView& buffer, bool sign)
	: buffer_view_(buffer), sign_(sign) { }

BigNumber::BigNumber(BigNumberBufferView&& buffer, bool sign)
	: buffer_view_(buffer), sign_(sign) { }

BigNumber BigNumber::operator+(const BigNumber& other) const {
	return add(other);
}
BigNumber BigNumber::operator-(const BigNumber& other) const {
	return minus(other);
}
BigNumber BigNumber::operator*(const BigNumber& other) const {
	return mult(other);
}
BigNumber BigNumber::operator<<(size_t count) const {
	return shift_left(count);
}
BigNumber BigNumber::operator/(const BigNumber& other) const {
	return div(other, nullptr);
}

bool BigNumber::is_zero() const {
	for (uint8_t byte : buffer_view_.data()) {
		if (byte != 0) return false;
	}

	return true;
}
bool BigNumber::operator== (const BigNumber& rhs) const {
	if (buffer_view_.exponent() < rhs.buffer_view_.exponent()) {
		return this->with_extended_buffer(rhs.buffer_view_.exponent()) == rhs;
	}

	return !compare_absolute_value(rhs) && sign_ == rhs.sign_;
}
bool BigNumber::operator < (const BigNumber& rhs) const {
	if (!sign_ && rhs.sign_) {
		return !is_zero() && !rhs.is_zero();
	}

	if (sign_ && rhs.sign_) {
		return !(rhs.negated() < this->negated());
	}

	return compare_absolute_value(rhs) == -1;
}
bool BigNumber::operator <= (const BigNumber& rhs) const {
	return *this < rhs || *this == rhs; // optimize
}


BigNumber BigNumber::div(const BigNumber& other, BigNumber* remainder) const {
	if (is_zero()) {
		if (remainder) *remainder = BigNumber(0u);
		return BigNumber(0u);
	}

	BigNumber q, r;

	// Divide by 256 and divide the result by other
	q = (this->shift_right(8)).div(other, &r);

	q = q << 8;
	r = r << 8;

	if (buffer_view_.data()[0]) {
		r = r + BigNumber(buffer_view_.data()[0]);
	}

	while (other <= r) {
		r = r - other;
		q = q + BigNumber(1u);
	}

	if (remainder) *remainder = r;
	return q;
}