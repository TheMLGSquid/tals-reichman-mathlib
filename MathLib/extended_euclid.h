#pragma once
#include <optional>

#include "BigNumber.h"
#include <cassert>
namespace algorithms {


struct ExtendedEuclidResult {
	BigNumber Gcd;
	BigNumber X;
	BigNumber Y;

	ExtendedEuclidResult(const BigNumber& gcd, const BigNumber& x, const BigNumber& y)
		: Gcd(gcd), X(x), Y(y) {}
};
ExtendedEuclidResult extended_euclid(const BigNumber& a, const BigNumber& b);
std::optional<BigNumber> mod_inverse(const BigNumber& k, const BigNumber& p);

} // namespace algorithms