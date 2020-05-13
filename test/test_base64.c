//
// BASE64 Performance test:
//	encoding:
//	  1,000,000 / 1.5s @64 bytes (667,000 TPS)
//	  1,000,000 / 5.7s @250 bytes (170,000 TPS)
//
//	decoding:
//	  1,000,000 / 1.8s @64 bytes (555,000 TPS)
//	  1,000,000 / 5.8s @250 bytes (170,000 TPS)
//


#include "base64.h"


int	main(int ac, char **av)
{
	string data60 = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAX";
	string data250 = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAX";

	printf("C++ code:\n\n");

	string data = data250;

	printf("DATA = %s\n", data.c_str());

	string enc64 = base64_encode(data);
	printf("ENCODE64 = %s (%ld)\n", enc64.c_str(), enc64.length());

	for (int ii = 0; ii < 1000000; ii++)
	{
		string enc64 = base64_encode(data);
	}

	string dec64 = base64_decode(enc64);
	printf("DECODE64 = %s (%ld)\n", dec64.c_str(), dec64.length());

//	for (int ii = 0; ii < 1000000; ii++)
	{
		string dec64 = base64_decode(enc64);
	}
}

