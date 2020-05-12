//
// Hdac code distributed under the GPLv3 license
//============================================================================================
// libhash.c
//	Hash functions
//
// History
//	2018/01/10	Initial code
//============================================================================================
//


#include "libhash.h"


int	sha1bin(char *outhash, const char *str, const size_t len)
{
	Sha1Context	ctx;
	SHA1_HASH	hash;

	Sha1Initialise(&ctx);
	Sha1Update(&ctx, str, len);
	Sha1Finalise(&ctx, &hash);

	memcpy(outhash, (void *)&hash, sizeof(hash));

	return sizeof(hash);
}


char	*sha1(char *outhash, const char *str, const size_t len)
{
	Sha1Context	ctx;
	SHA1_HASH	hash;

	Sha1Initialise(&ctx);
	Sha1Update(&ctx, str, len);
	Sha1Finalise(&ctx, &hash);

	return bin2hex(outhash, (void *)&hash, sizeof(hash));
}


int	sha256bin(char *outhash, const char *str, const size_t len)
{
	Sha256Context	ctx;
	SHA256_HASH	hash;

	Sha256Initialise(&ctx);
	Sha256Update(&ctx, str, len);
	Sha256Finalise(&ctx, &hash);

	memcpy(outhash, (void *)&hash, sizeof(hash));

	return sizeof(hash);
}


char	*sha256(char *outhash, const char *str, const size_t len)
{
	Sha256Context	ctx;
	SHA256_HASH	hash;

	Sha256Initialise(&ctx);
	Sha256Update(&ctx, str, len);
	Sha256Finalise(&ctx, &hash);

	return bin2hex(outhash, (void *)&hash, sizeof(hash));
}


int	sha512bin(char *outhash, const char *str, const size_t len)
{
	Sha512Context	ctx;
	SHA512_HASH	hash;

	Sha512Initialise(&ctx);
	Sha512Update(&ctx, str, len);
	Sha512Finalise(&ctx, &hash);

	memcpy(outhash, (void *)&hash, sizeof(hash));

	return sizeof(hash);
}


char	*sha512(char *outhash, const char *str, const size_t len)
{
	Sha512Context	ctx;
	SHA512_HASH	hash;

	Sha512Initialise(&ctx);
	Sha512Update(&ctx, str, len);
	Sha512Finalise(&ctx, &hash);

	return bin2hex(outhash, (void *)&hash, sizeof(hash));
}


char	*md5(char *outhash, const char *str, const size_t len)
{
	Md5Context	ctx;
	MD5_HASH	hash;

	Md5Initialise(&ctx);
	Md5Update(&ctx, str, len);
	Md5Finalise(&ctx, &hash);

	return bin2hex(outhash, (void *)&hash, sizeof(hash));
}


char	*bin2hex(char *hexbuf, const void *binbuf, const size_t binbuflen)
{
	char	*sp = (char *)binbuf, *dp = hexbuf;
	int	ii = 0;

	for (ii = 0; ii < binbuflen; ii++)
	{
		int	cc = (*sp & 0x00F0) >> 4;

		cc = (*sp & 0x00F0) >> 4;
		*dp++ = TOHEXDIGIT(cc);
		cc = (*sp & 0x000F);
		*dp++ = TOHEXDIGIT(cc);
		sp++;
	}
	*dp = 0;

	return hexbuf;
}


void	*hex2bin(void *binbuf, const char *hexbuf, const size_t hexbuflen)
{
	char	*sp = (char *)hexbuf, *dp = (char *) binbuf;
	int	ii = 0;

	for (ii = 0; ii < hexbuflen; ii += 2)
	{
		int	hh = tolower(sp[ii]), ll = tolower(sp[ii+1]);

		if (hh >= 'a' && hh <= 'f')
			hh = hh - 'a' + 10;
		else
			hh = hh - '0';

		if (ll >= 'a' && ll <= 'f')
			ll = ll - 'a' + 10;
		else
			ll = ll - '0';
		*dp++ = (hh << 4) | ll;
	}
	*dp = 0;

	return binbuf;
}
