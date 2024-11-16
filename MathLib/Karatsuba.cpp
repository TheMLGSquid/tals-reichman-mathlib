#include "BigNumber.h"
#include <iostream>
#include <iomanip>
#include <random>

int main() {
	std::cout << BigNumber(166u) / BigNumber(5u) << std::endl;
	return 0;
	BigNumberBuffer a(12), b(12); // 4096 bit numbers

	std::random_device rd;  // a seed source for the random number engine
	std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution dist(0, 255);

	for (int i = 0; i < a.data().size() - 1; i++) {
		a.data()[i] = dist(gen);
		b.data()[i] = dist(gen);
	}

	BigNumber x = BigNumber(BigNumberBufferView(std::move(a))),
		y = BigNumber(BigNumberBufferView(std::move(b)));

	std::cout << x << " Times " << y << std::endl;

	std::cout << "-----------------------------------------------\n";

	std::cout << x * y << std::endl;
}