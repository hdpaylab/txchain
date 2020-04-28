#include "bigint.hpp"

#include <assert.h>
#include <iostream>

uint32_t xorshift32();
void not_secure_random(uint8_t *dst, int n);
void max_value(void);

int main(){

	// ================ example ================

	// BigInts can be created from strings or from integers
	BigInt a = "-1137531041259095389425522063651335971086542522289";
	BigInt b = "-9214001518046086468566115579527473139501";

	uint32_t k = 100;
	uint64_t l = 200;

	BigInt r = k;
	BigInt e = l;

	BigInt q = e - r;

	q.write(std::cout) << std::endl;

	// Available operators:
	// +, -, *, /, %, <<, >>
	// +=, -=, *=, /=, %=, <<=, >>=, ++, --
	// ==, !=, <=, >=, <, >
	BigInt c = a / b;
	BigInt d = b * c;

	// write to any output stream
	c.write(std::cout) << std::endl;
	d.write(std::cout) << std::endl;

	max_value();

	// ================ example ================

	int i = 0;
	clock_t start;
        clock_t end;
        double cpuTime;
	
	start = clock();

	for (i = 0; i < 100000; i++) {
		BigInt c = a / b;
		BigInt d = b * c;
	}

	end = clock();
        cpuTime = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("%d loop [%f] seconds.\n", i, cpuTime);

	return 0;
}

void max_value(void) 
{
	// find the biggest probable prime less than 10^42
	BigInt p = BigInt(10).pow(42) - 1;

	for (int i = 0; i < 100; i++){
		if (p.is_probable_prime(10, not_secure_random)){
			p.write(std::cout << "Big prime: ") << std::endl;
			break;
		}
		--p;
	}
}

uint32_t xorshift32() {
	static uint32_t x = 314159265;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return x;
}

// do not use this as a cryptographically secure random number generator
void not_secure_random(uint8_t *dst, int n){
	for (int i = 0; i < n; i++) dst[i] = xorshift32();
}


