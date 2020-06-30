#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include "rsa.h"

void print_n(const void *_data, size_t len)
{
	const uint8_t *data = (const uint8_t *)_data;
	size_t i;
	for (i = 0; i < len; ++i) {
		printf("%02x", (int)data[i]);
	}
}


RSA	*createRSA(unsigned char * key,int public)
{
	RSA *rsa= NULL;
	BIO *keybio ;
	keybio = BIO_new_mem_buf(key, strlen(key));

	BIO_set_flags(keybio, BIO_FLAGS_BASE64_NO_NL);
	if (keybio==NULL)
	{
		printf( "Failed to create key BIO");
		return 0;
	}
	if(public)
	{
		rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
	}
	else
	{
		rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
	}
	if(rsa == NULL)
	{
		printf( "Failed to create RSA");
	}

	return rsa;
}

int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
	RSA *rsa = createRSA(key,1);
	assert(rsa != NULL);
	int result = RSA_public_encrypt(data_len,data,
				encrypted,rsa,RSA_PKCS1_PADDING);
	return result;
}

// X86 only: get CPU clock 
unsigned long long rdtsc(void)
{
	unsigned int low, high;
	asm volatile("rdtsc" : "=a" (low), "=d" (high));
	return low | ((unsigned long long)high) << 32;
}


double	xgetclock()
{
	struct timeval tv;
	double	dv = 0;

	gettimeofday(&tv, NULL);
	dv = tv.tv_sec + tv.tv_usec / 1000000.0;

	return dv;
}


//
// 甕걔1獄紐甕걔嚥≤ 筌◈
// x86 ㉱ cpu봔 野 clockicro-second 揶 XOR   
//  1獄紐筌◈ 節. 64獄= 10^135甕걔嚥≤ 겸/
uint8_t	*getrandom(uint8_t *buf, int buflen)
{
	if (buf == NULL || buflen <= 0)
		return NULL;

	// system  甕걔 ..
	char randbuf[128] = {0};
	int bytes = 0, fd = open("/dev/urandom", O_RDONLY);
	if (fd > 0)
	{
		bytes = read(fd, randbuf, sizeof(randbuf));
		printf("/dev/urandom="); print_n(randbuf, sizeof(randbuf)); printf("\n");
		close(fd);
	}

	// Random number嚥≤ 굿 	int     ii = 0, clock_enable = 0;
	unsigned long long clk = 0;
	struct timeval tv = {0};

	// 甕걔 Β疫꿸. srand()嚥≤ rand() Β Β疫꿸 
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec ^ rand());

	int clock_enable = 0;

	// x86 CPU clock 筌筌셋 Β疫꿸..
	clk = rdtsc();
	if (clk > 0)
	{
		srand(rdtsc() & 0x0FFFFFFFFL);
		clock_enable = 1;
	}

	int ii = 0;

	//  獄紐甕걔 (clock 삘ㅵ 삥에 筌ι봔揶)
	for (ii = 0; ii < buflen; ii++)
	{
		gettimeofday(&tv, NULL);	// 筌Β 
		if (clock_enable > 0)
		{
			clk = rdtsc();
			buf[ii] = (randbuf[ii] ^ clk ^ tv.tv_usec ^ rand()) & 0x00FF;
		}
		else
		{
			buf[ii] = (randbuf[ii] ^ tv.tv_usec ^ rand()) & 0x00FF;
		}
		//		printf("rand[%2d]= %lld = 0x%llx tv=%ld rand=[%02X]\n",
		//			ii, clk, clk, tv.tv_usec, buf[ii]);
	}
	printf("\n");

	// LITTLE/BIG endian 筌Ｂ굿 (RAND굿  ?)
	printf("RAND="); print_n(buf, buflen); printf("\n");
	printf("\n");

	return buf;
}
