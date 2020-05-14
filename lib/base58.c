/*
 * Copyright 2012-2014 Luke Dashjr
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the standard MIT license.  See COPYING for more details.
 */

#ifndef WIN32
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>


#include "base58.h"


bool (*sha256_impl)(void *, const void *, size_t) = NULL;


static const int8_t b58digits_map[] = {
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
	-1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
	22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
	-1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
	47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
};


bool base58_decode(void *outbin, size_t *outbinszp, const char *b58, size_t b58sz)
{
	size_t	binsz = *outbinszp, orgsz = *outbinszp;
	const unsigned char *b58u = (const unsigned char *) b58;
	unsigned char *binu = (unsigned char *) outbin;
	size_t outisz = (binsz + 3) / 4;
	uint32_t outi[outisz];
	uint64_t t;
	uint32_t c;
	size_t i, j;
	uint8_t bytesleft = binsz % 4;
	uint32_t zeromask = bytesleft ? (0xffffffff << (bytesleft * 8)) : 0;
	unsigned zerocount = 0;
	
	if (!b58sz)
		b58sz = strlen(b58);
	
	memset(outi, 0, outisz * sizeof(*outi));
	
	// Leading zeros, just count
	for (i = 0; i < b58sz && b58u[i] == '1'; ++i)
		++zerocount;
	
	for ( ; i < b58sz; ++i)
	{
		// High-bit set on invalid digit
		if (b58u[i] & 0x80)
		{
		//	printf("ERROR: High-bit set on invalid digit: i=%d 0x%02X\n", i, b58u[i] & 0x00FF);
			return false;
		}
		// Invalid base58 digit
		if (b58digits_map[b58u[i]] == -1)
		{
		//	printf("ERROR: Invalid base58 digit: i=%d 0x%02X\n", i, b58u[i] & 0x00FF);
			return false;
		}
		c = (unsigned)b58digits_map[b58u[i]];
		for (j = outisz; j--; )
		{
			t = ((uint64_t)outi[j]) * 58 + c;
			c = (t & 0x3f00000000) >> 32;
			outi[j] = t & 0xffffffff;
		}
		// Output number too big (carry to the next int32)
		if (c)
		{
		//	printf("ERROR: Output number too big (carry to the next int32): i=%d\n", i);
			return false;
		}
		// Output number too big (last int32 filled too far)
		if (outi[0] & zeromask)
		{
		//	printf("ERROR: Output number too big (last int32 filled too far): i=%d\n", i);
			return false;
		}
	}

	j = 0;
	switch (bytesleft) {
		case 3:
			*(binu++) = (outi[0] &   0xff0000) >> 16;
		case 2:
			*(binu++) = (outi[0] &     0xff00) >>  8;
		case 1:
			*(binu++) = (outi[0] &       0xff);
			++j;
		default:
			break;
	}
	
	for (; j < outisz; ++j)
	{
		*(binu++) = (outi[j] >> 0x18) & 0xff;
		*(binu++) = (outi[j] >> 0x10) & 0xff;
		*(binu++) = (outi[j] >>    8) & 0xff;
		*(binu++) = (outi[j] >>    0) & 0xff;
	}
	
	// Count canonical base58 byte count
	binu = (unsigned char *) outbin;
	for (i = 0; i < binsz; ++i)
	{
		if (binu[i])
			break;
		--*outbinszp;
	}
	*outbinszp += zerocount;

	// 문자열 처음부터 오도록 수정..
	for (i = 0; i < *outbinszp; i++)
	{
		binu[i] = binu[orgsz - *outbinszp + i];
	}
	binu[i] = 0;
	
	return true;
}


static	bool	my_dblsha256(void *hash, const void *data, size_t datasz)
{
	uint8_t buf[0x20];
	return sha256_impl(buf, data, datasz) && sha256_impl(hash, buf, sizeof(buf));
}


int	base58_check(const void *outbin, size_t binsz, const char *base58str, size_t b58sz)
{
	unsigned char buf[32];
	const uint8_t *binc = (const uint8_t *) outbin;
	unsigned i;
	if (binsz < 4)
		return -4;
	if (!my_dblsha256(buf, outbin, binsz - 4))
		return -2;
	if (memcmp(&binc[binsz - 4], buf, 4))
		return -1;
	
	// Check number of zeros is correct AFTER verifying checksum 
	// (to avoid possibility of accessing base58str beyond the end)
	for (i = 0; binc[i] == '\0' && base58str[i] == '1'; ++i)
	{}  // Just finding the end of zeros, nothing to do in loop
	if (binc[i] == '\0' || base58str[i] == '1')
		return -3;
	
	return binc[0];
}


static const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";


bool	base58_encode(char *outb58, size_t *outb58sz, const void *data, size_t binsz)
{
	const uint8_t *bin = (const uint8_t *) data;
	int carry;
	ssize_t i, j, high, zcount = 0;
	size_t size;
	
	while (zcount < (ssize_t)binsz && !bin[zcount])
		++zcount;
	
	size = (binsz - zcount) * 138 / 100 + 1;
	uint8_t buf[size];
	memset(buf, 0, size);
	
	for (i = zcount, high = size - 1; i < (ssize_t)binsz; ++i, high = j)
	{
		for (carry = bin[i], j = size - 1; (j > high) || carry; --j)
		{
			carry += 256 * buf[j];
			buf[j] = carry % 58;
			carry /= 58;
		}
	}
	
	for (j = 0; j < (ssize_t)size && !buf[j]; ++j);
	
	if (*outb58sz <= zcount + size - j)
	{
		*outb58sz = zcount + size - j + 1;
		return false;
	}
	
	if (zcount)
		memset(outb58, '1', zcount);
	for (i = zcount; j < (ssize_t)size; ++i, ++j)
	{
		outb58[i] = b58digits_ordered[buf[j]];
	}
	outb58[i] = '\0';
	*outb58sz = i + 1;
	
	return true;
}

bool	base58_check_encode(char *outb58c, size_t *outb58c_sz, uint8_t ver, const void *data, size_t datasz)
{
	uint8_t buf[1 + datasz + 0x20];
	uint8_t *hash = &buf[1 + datasz];
	
	buf[0] = ver;
	memcpy(&buf[1], data, datasz);
	if (!my_dblsha256(hash, buf, datasz + 1))
	{
		*outb58c_sz = 0;
		return false;
	}
	
	return base58_encode(outb58c, outb58c_sz, buf, 1 + datasz + 4);
}



string base58_encode(string instr)
{
	size_t	outb58sz = instr.length() * 2;
	char	*outb58 = (char *) calloc(1, outb58sz);
	string	null;

	if (outb58 == NULL)
		return null;

	bool ret = base58_encode(outb58, &outb58sz, instr.c_str(), instr.length());

	if (ret)
	{
		string retstr(outb58, outb58sz);
		free(outb58);

		return retstr;
	}
	else
	{
		string retstr;
		free(outb58);

		return retstr;
	}
}


string base58_decode(string instr)
{
	size_t	outbinsz = instr.length();
	char	*outbin = (char *) calloc(1, outbinsz + 1);
	string	null;

	if (outbin == NULL)
		return null;

	// instr.length()로 하면 마지막 0x00이 포함되어 오류 출력됨 
	bool ret = base58_decode(outbin, &outbinsz, instr.c_str(), strlen(instr.c_str()));

	if (ret)
	{
		string retstr(outbin, outbinsz);
		free(outbin);

		return retstr;
	}
	else
	{
		string retstr;
		free(outbin);

		return retstr;
	}
}


