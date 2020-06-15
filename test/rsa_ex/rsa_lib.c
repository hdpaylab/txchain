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

RSA *createRSA(unsigned char * key,int public)
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

// X86 only: CPU clock 쎌댁ㅺ린..
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
// 踰쇰 1諛댄몄踰濡 梨
// x86 怨댁 cpu瑜ъ⑺ 寃쎌 clock怨micro-second 媛 XOR   
//  1諛댄몄梨 ｌ. 64諛댄= 10^135쇰 踰濡 異⑸//
uint8_t	*getrandom(uint8_t *buf, int buflen)
{
	if (buf == NULL || buflen <= 0)
		return NULL;

	// system  踰 ..
	char randbuf[128] = {0};
	int bytes = 0, fd = open("/dev/urandom", O_RDONLY);
	if (fd > 0)
	{
		bytes = read(fd, randbuf, sizeof(randbuf));
		printf("/dev/urandom="); print_n(randbuf, sizeof(randbuf)); printf("\n");
		close(fd);
	}

	// Random number濡 쇱대 	int     ii = 0, clock_enable = 0;
	unsigned long long clk = 0;
	struct timeval tv = {0};

	// 踰 珥湲고. srand()濡 rand() ⑥ 珥湲고 
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec ^ rand());

	int clock_enable = 0;

	// x86 CPU clock 吏硫ㅼ 珥湲고..
	clk = rdtsc();
	if (clk > 0)
	{
		srand(rdtsc() & 0x0FFFFFFFFL);
		clock_enable = 1;
	}

	int ii = 0;

	//  諛댄몄踰 (clock ㅻⅤ寃 ㅻ濡 痢遺媛)
	for (ii = 0; ii < buflen; ii++)
	{
		gettimeofday(&tv, NULL);	// 留댄щ珥 ъ
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

	// LITTLE/BIG endian 泥由 (RAND쇱  ?)
	printf("RAND="); print_n(buf, buflen); printf("\n");
	printf("\n");

	return buf;
}
