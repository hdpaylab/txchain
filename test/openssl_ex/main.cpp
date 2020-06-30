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
#include "xrsa.h"

using namespace std;


void sign_verify_test(xrsa& rsa);
void enc_dec_test(xrsa& rsa);
void evp_test(xrsa& rsa);
string read_file(const char *filename);


int main(int argc, const char *argv[])
{
	struct stat st;
	rsakey *key = new rsakey();

	key->generateNewKey("pub.pem", "pri.pem");
	xrsa *rsa = new xrsa(key);

	sign_verify_test(*rsa);

	enc_dec_test(*rsa);

	evp_test(*rsa);
}


void sign_verify_test(xrsa& rsa)
{
	uchar *inbuf = NULL;
	uint inbufsz = 0;
	uchar *signature = NULL;
	uint signlen = 0;

	////////////////////////////////////////////////////////////////////////////////
	printf("Signing...\n");

	string plaintext = read_file("_input.txt");
	assert(plaintext.size() > 0);

	if (rsa.sign((uchar *)plaintext.c_str(), plaintext.size(), (uchar **) &signature, signlen) <= 0)
		perror("Signature Error!");

	printf("    sign ok. sign length=%d / total length=%ld / signed file=_signture.txt\n", 
		signlen, signlen + plaintext.size());

	////////////////////////////////////////////////////////////////////////////////
	printf("Verification...\n");

	int res = rsa.verify(signature, 128, (uchar *)plaintext.c_str(), plaintext.size());
	printf("    verification %s!\n\n", res == 1 ? "ok" : "FAILED!");

	// save signature
	FILE *signfp = fopen("_signture.txt", "wb");
	assert(signfp != NULL);

	fwrite(plaintext.c_str(), 1, plaintext.size(), signfp);		// 파일 내용 기록 
	fwrite(signature, 1, signlen, signfp);		// 사인 기록 

	fclose(signfp);
}


void enc_dec_test(xrsa& rsa)
{
	struct stat st;
	uchar *inbuf = NULL;
	uint inbufsz = 0;


	////////////////////////////////////////////////////////////////////////////////
	// Encrypt 

	printf("Encryption...\n");

	string plaintext = read_file("_input.txt");
	assert(plaintext.size() > 0);

	FILE *encfp = fopen("_encrypted.txt", "wb");
	assert(stat("_encrypted.txt", &st) >= 0);

	string encdata = rsa.encrypt_string(plaintext);		// 암호화 

	fwrite(encdata.c_str(), 1, encdata.size(), encfp);

	fclose(encfp);

	printf("    encryption ok. length=%ld encrypt file=_encrypted.txt\n\n", plaintext.size() / 100 * 128);


	////////////////////////////////////////////////////////////////////////////////
	// Decrypt 

	printf("Decryption...\n");

	FILE *decfp = fopen("_decrypted.txt", "wb");
	assert(decfp != NULL);

	string decdata = rsa.decrypt_string(encdata);	// 복호화 

	fwrite(decdata.c_str(), 1, decdata.size(), decfp);

	fclose(decfp);

	if (decdata == plaintext)
		printf("    decryption ok. size=%ld decrypt file=_decrypted.txt\n\n", decdata.size());
	else
		printf("    decryption FAILED!. size=%ld decrypt file=_decrypted.txt\n\n", decdata.size());
}


void evp_test(xrsa& rsa)
{
	struct stat st;
	uchar *inbuf = NULL;
	uint inbufsz = 0;


	////////////////////////////////////////////////////////////////////////////////
	// Encrypt 

	printf("EVP Encryption...\n");

	string plaintext = read_file("_input.txt");
	assert(plaintext.size() > 0);

	FILE *encfp = fopen("_evp_encrypt.txt", "wb");
	assert(stat("_encrypted.txt", &st) >= 0);

	string encdata = rsa.evp_encrypt_string(plaintext);		// 암호화 

	fwrite(encdata.c_str(), 1, encdata.size(), encfp);

	fclose(encfp);

	printf("    encryption ok. size=%ld==%d encrypt file=_encrypted.txt\n\n", 
		encdata.size(), inbufsz / 100 * 128);


	////////////////////////////////////////////////////////////////////////////////
	// Decrypt 

	printf("EVP Decryption...\n");

	FILE *decfp = fopen("_evp_decrypt.txt", "wb");
	assert(decfp != NULL);

	string decdata = rsa.evp_decrypt_string(encdata);	// 복호화 

	fwrite(decdata.c_str(), 1, decdata.size(), decfp);

	fclose(decfp);

	if (decdata == plaintext)
		printf("    decryption ok. size=%ld decrypt file=_evp_decrypt.txt\n\n", decdata.size());
	else
		printf("    decryption FAILED!. size=%ld decrypt file=_evp_decrypt.txt\n\n", decdata.size());
}


string read_file(const char *filename)
{
	struct stat st;

	printf("    reading: %s\n", filename);

	assert(stat(filename, &st) >= 0);
	FILE *infp = fopen(filename, "rb");
	assert(infp != NULL);

	size_t	bufsz = st.st_size;
	char	*buf = (char *) calloc(1, bufsz);
	assert(buf != NULL);

	if (fread(buf, 1, bufsz, infp) != bufsz)
	{
		perror("File load error!");
		exit(0);
	}
	fclose(infp);

	printf("    file read ok. size=%ld\n", bufsz);

	string readbuf(buf, bufsz);

	return readbuf;
}


