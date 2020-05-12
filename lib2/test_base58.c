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


int	main(int ac, char **av)
{
	char	bin[2048] = {0}, tmp[2048] = {0};
	char	b58[2048] = {0};
	char	outbin[2048] = {0};
	size_t	b58sz = 0, outbinsz = 0;

	if (ac <= 1)
		strcpy(bin, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	else
		strcpy(bin, av[1]);

	b58sz = sizeof(b58);
//	for (int nn = 0; nn < 1000; nn++)
	{
	base58_encode(b58, &b58sz, bin, strlen(bin));
	}
	printf("base58_encode:\nout=%s b58sz=%ld\nbin=%s (%ld)\n\n", 
		b58, b58sz, bin, strlen(bin));
	for (int nn = 0; nn < b58sz; nn++)
	{
		printf("[%2d] = 0x%02X ", nn, b58[nn]);
	}
	printf("\nEncoding finished!\n\n");

	outbinsz = sizeof(outbin);
//	for (int nn = 0; nn < 1000000; nn++)
	{
	base58_decode(outbin, &outbinsz, b58, strlen(b58));
	}
	printf("base58_decode: b58sz=%ld out=%s (%ld)\n\n",
		b58sz, outbin, strlen(outbin));

	if (outbinsz != strlen(bin) || strcmp(bin, outbin) != 0)
		printf("Base58: Error!\n");
	else
		printf("Base58: Passed!\n");

	return 0;
}

