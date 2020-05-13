//
// Performance test:
//	encoding:
//	  1,000,000 / 4.9s @52 bytes (4.9 us)
//	  100,000   / 10.7s @250 bytes (107 us)
//	  1,000     / 1.6s @1000 bytes (1600 us)
//
//	decoding:
//	  1,000,000 / 0.9s @52 bytes (0.9 us)
//	  100,000   / 2.0s @250 bytes (20 us)
//	  1,000	    / 0.3s @1000 bytes (300 us)
//


#include "libhash.h"


void	test_c();
void	test_cpp();


int	main(int ac, char **av)
{
	test_c();

	test_cpp();
}


void	test_c()
{
	char	bin[2048] = {0};
	char	b58[2048] = {0};
	char	outbin[2048] = {0};
	size_t	b58sz = 0, outbinsz = 0;

	printf("C code:\n\n");

	strcpy(bin, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAX");

	b58sz = sizeof(b58);
	for (int nn = 0; nn < 100000; nn++)
	{
		base58_encode(b58, &b58sz, bin, strlen(bin));
	}
	printf("base58_encode:\nENCODE58 = %s (%ld)\nDATA = %s (%ld)\n\n", 
		b58, b58sz, bin, strlen(bin));
	for (int nn = 0; nn < (int)b58sz; nn++)
	{
//		printf("[%2d] = 0x%02X ", nn, b58[nn]);
	}
	printf("\nEncoding finished!\n\n");

	outbinsz = sizeof(outbin);
//	for (int nn = 0; nn < 100000; nn++)
	{
		base58_decode(outbin, &outbinsz, b58, strlen(b58));
	}
	printf("base58_decode: SIZE = %ld DECODE58 = %s (%ld)\n\n",
		b58sz, outbin, strlen(outbin));

	if (outbinsz != strlen(bin) || strcmp(bin, outbin) != 0)
		printf("Base58: Error!\n");
	else
		printf("Base58: Passed!\n");
}


void	test_cpp()
{
	string data = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAX";

	printf("C++ code:\n\n");

	printf("DATA = %s\n", data.c_str());

	string enc58 = base58_encode(data);
	printf("ENCODE58 = %s (%ld)\n", enc58.c_str(), enc58.length());

	string dec58 = base58_decode(enc58);
	printf("DECODE58 = %s (%ld)\n", dec58.c_str(), dec58.length());
}

