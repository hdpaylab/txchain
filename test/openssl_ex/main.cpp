//
//  main.cpp
//  openssl	sign: 3800 TPS @i9900
//		verify: 35000 TPS @i9900
//
//  Created by Xinbao Dong on 15/4/4.
//  Copyright (c) 2015年 com.dongxinbao. All rights reserved.
//

#include <iostream>
#include <sys/stat.h>
#include <assert.h>
#include "myrsa.h"

using namespace std;


void sign_verify(myrsa *rsa);
int read_file(const char *filename, uchar **buf, uint *bufsz);
void enc_dec_test(myrsa *rsa);
void evp_test(myrsa *rsa);


int main(int argc, const char *argv[])
{
	struct stat st;
	Key *key = new Key();

	key->generateNewKey("pub.pem", "pri.pem");
	myrsa *rsa = new myrsa(key);

	sign_verify(rsa);

	enc_dec_test(rsa);

	evp_test(rsa);
}


void sign_verify(myrsa *rsa)
{
	uchar *inbuf = NULL;
	uint inbufsz = 0;
	uchar *signature = NULL;
	uint signlen = 0;

	printf("Signing...\n");

	assert(read_file("_input.txt", &inbuf, &inbufsz) > 0);

	if (rsa->sign((uchar *)inbuf, inbufsz, (uchar **) &signature, signlen) <= 0)
		perror("Signature Error!");

	printf("    sign ok. sign length=%d / total length=%d / signed file=_signture.txt\n", 
		signlen, signlen + inbufsz);

	printf("Verification...\n");
	int res = rsa->verify(signature, 128, inbuf, inbufsz);
	printf("    verification %s!\n\n", res == 1 ? "ok" : "failed!");

	// save signature
	FILE *signfp = fopen("_signture.txt", "wb");
	assert(signfp != NULL);

	fwrite(inbuf, 1, inbufsz, signfp);		// 파일 내용 기록 
	fwrite(signature, 1, signlen, signfp);		// 사인 기록 
	fclose(signfp);
}


void enc_dec_test(myrsa *rsa)
{
	struct stat st;
	uchar *inbuf = NULL;
	uint inbufsz = 0;

	////////////////////////////////////////////////////////////////////////////////
	// Encrypt 

	printf("Encryption...\n");

	assert(read_file("_input.txt", &inbuf, &inbufsz) > 0);

	FILE *encfp = fopen("_encrypted.txt", "wb");
	assert(stat("_encrypted.txt", &st) >= 0);

	uchar *encbuf = NULL;
	uint encbufsz = 0;
	int i = 0;

	while (i < inbufsz)
	{
		if (rsa->encrypt((uchar *) (inbuf + i), 100, (uchar **) &encbuf, encbufsz) <= 0)
		{
			perror("Signed File encrypt error!");
			exit(0);
		}
		fwrite(encbuf, 1, encbufsz, encfp);

		free(encbuf);
		i += 100;	//100은 128-11보다 작으면 암호화 단위입니다.
	}
	fclose(encfp);

	printf("    encryption ok. length=%d encrypt file=_encrypted.txt\n\n", inbufsz / 100 * 128);


	////////////////////////////////////////////////////////////////////////////////
	// Decrypt 

	printf("Decryption...\n");

	assert(read_file("_encrypted.txt", &encbuf, &encbufsz) > 0);

	FILE *decfp = fopen("_decrypted.txt", "wb");
	assert(decfp != NULL);

	//save the uncrypted data to file
	char *tt = NULL;
	unsigned int len2 = 0, wbytes = 0;

	i = 0;
	while (i < encbufsz)
	{
		int y = rsa->decrypt((uchar *) encbuf + i, 128, (uchar **) &tt, len2);

		// remove space
		if (i + 128 >= encbufsz)
		{
			y = 100;
			while (tt[y - 1] == 0)
			{
				y--;
				if (y == 0)
				{
					break;
				}
			}
		}
		wbytes += fwrite(tt, 1, y, decfp);
		free(tt);
		i += 128;
	}
	fclose(decfp);

	printf("     decryption ok. size=%d\n\n", wbytes);
}


int read_file(const char *filename, uchar **buf, uint *bufsz)
{
	struct stat st;

	printf("    reading: %s\n", filename);

	assert(stat(filename, &st) >= 0);
	FILE *infp = fopen(filename, "rb");
	assert(infp != NULL);

	*bufsz = st.st_size;
	*buf = (uchar *) calloc(1, *bufsz);
	assert(*buf != NULL);

	if (fread(*buf, 1, *bufsz, infp) != *bufsz)
	{
		perror("File load error!");
		exit(0);
	}
	fclose(infp);

	printf("    file read ok. size=%d\n", *bufsz);

	return *bufsz;
}


void evp_test(myrsa *rsa)
{
	struct stat st;
	uchar *inbuf = NULL;
	uint inbufsz = 0;

	////////////////////////////////////////////////////////////////////////////////
	// Encrypt 

	printf("EVP Encryption...\n");

	assert(read_file("_input.txt", &inbuf, &inbufsz) > 0);

	FILE *encfp = fopen("_evp_encrypt.txt", "wb");
	assert(stat("_encrypted.txt", &st) >= 0);

	uchar *encbuf = NULL;
	uint encbufsz = 0;
	int i = 0, wbytes = 0;

	while (i < inbufsz)
	{
		if (rsa->evp_encrypt((uchar *) (inbuf + i), 100, (uchar **) &encbuf, encbufsz) <= 0)
		{
			perror("Signed File encrypt error!");
			exit(0);
		}
		wbytes += fwrite(encbuf, 1, encbufsz, encfp);

		free(encbuf);
		i += 100;	//100은 128-11보다 작으면 암호화 단위입니다.
	}
	fclose(encfp);

	printf("    encryption ok. size=%d==%d encrypt file=_encrypted.txt\n\n", 
		wbytes, inbufsz / 100 * 128);


	////////////////////////////////////////////////////////////////////////////////
	// Decrypt 

	printf("EVP Decryption...\n");

	assert(read_file("_evp_encrypt.txt", &encbuf, &encbufsz) > 0);

	FILE *decfp = fopen("_evp_decrypt.txt", "wb");
	assert(decfp != NULL);

	//save the uncrypted data to file
	char *tt = NULL;
	unsigned int len2 = 0;

	i = wbytes = 0;
	while (i < encbufsz)
	{
		int y = rsa->evp_decrypt((uchar *) encbuf + i, 128, (uchar **) &tt, len2);

		// remove space
		if (i + 128 >= encbufsz)
		{
			y = 100;
			while (tt[y - 1] == 0)
			{
				y--;
				if (y == 0)
				{
					break;
				}
			}
		}
		wbytes += fwrite(tt, 1, y, decfp);
		free(tt);
		i += 128;
	}
	fclose(decfp);

	printf("    decryption ok. size=%d\n\n", wbytes);
}
