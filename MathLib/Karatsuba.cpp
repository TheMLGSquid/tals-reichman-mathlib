#include "BigNumber.h"
#include <iostream>
#include <iomanip>
#include <random>

void multiplication_demo() {
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
void division_demo() {
	BigNumberBuffer a(9), b(9); // 2^9 = 512 bit numbers

	std::random_device rd;  // a seed source for the random number engine
	std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution dist(0, 255);

	for (int i = 0; i < a.data().size(); i++) {
		a.data()[i] = dist(gen);
	}
	for (int i = 0; i < b.data().size(); i++) {
		b.data()[i] = dist(gen);
	}

	BigNumber x = BigNumber(BigNumberBufferView(std::move(a))),
		y = BigNumber(BigNumberBufferView(std::move(b)));


	std::cout << x << " divided by " << y << std::endl;

	std::cout << "-----------------------------------------------\n";

	BigNumber remainder;
	BigNumber quotient = x.div(y, &remainder);

	std::cout << "Quotient: " << quotient << std::endl;

	std::cout << "Remainder: " << remainder << std::endl;
}

int main() {
	division_demo();
}