#include "extended_euclid.h"
namespace algorithms {

ExtendedEuclidResult extended_euclid(const BigNumber& a, const BigNumber& b) {
	assert(a.non_negative() && b.non_negative());

	if (b.is_zero()) {
		return ExtendedEuclidResult(a, BigNumber::ONE, BigNumber::ZERO);
	}

	BigNumber r;
	BigNumber q = a.div(b, &r);

	ExtendedEuclidResult res = extended_euclid(b, std::move(r));

	return ExtendedEuclidResult(res.Gcd, res.Y, res.X - (q * res.Y));
}

std::optional<BigNumber> mod_inverse(const BigNumber& k, const BigNumber& p) {
	ExtendedEuclidResult res = extended_euclid(k, p);

	if (res.Gcd != BigNumber::ONE) {
		return {};
	}

	return res.X.mod(p);
}
}