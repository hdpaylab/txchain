//
//  myrsa.cpp
//  openssl
//
//  Created by Xinbao Dong on 15/4/5.
//  Copyright (c) 2015年 com.dongxinbao. All rights reserved.
//

#include "myrsa.h"
#include <string.h>
#include <openssl/evp.h>


myrsa::myrsa(Key * key)
{
	this->key = key;
}


int myrsa::encrypt(const unsigned char *plain, int length, unsigned char **cipher, uint &r_length)
{
	key->reload();
	r_length = RSA_size(key->privateKey);
	*cipher = (unsigned char *) malloc(sizeof(char) * r_length);
	memset((void *) *cipher, 0, r_length);
	int res = RSA_private_encrypt(length, (unsigned char *) plain, (unsigned char *) *cipher, key->privateKey, RSA_PKCS1_PADDING);

	return res;
}

int myrsa::decrypt(const unsigned char *cipher, int length, unsigned char **plain, uint &r_length)
{
	key->reload();
	r_length = RSA_size(key->publicKey);
	*plain = (unsigned char *) malloc(sizeof(char) * r_length);
	memset((void *) *plain, 0, r_length);
	return RSA_public_decrypt(length, (unsigned char *) cipher, (unsigned char *) *plain, key->publicKey, RSA_PKCS1_PADDING);
}


int myrsa::evp_encrypt(const unsigned char *plain, int length, unsigned char **cipher, uint &r_length)
{
	key->reload();
	r_length = RSA_size(key->publicKey);
	*cipher = (unsigned char *) malloc(sizeof(char) * r_length);
	memset((void *) *cipher, 0, r_length);
	int res = RSA_public_encrypt(length, (unsigned char *) plain, (unsigned char *) *cipher, key->publicKey, RSA_PKCS1_PADDING);

	return res;
}

int myrsa::evp_decrypt(const unsigned char *cipher, int length, unsigned char **plain, uint &r_length)
{
	key->reload();
	r_length = RSA_size(key->privateKey);
	*plain = (unsigned char *) malloc(sizeof(char) * r_length);
	memset((void *) *plain, 0, r_length);
	return RSA_private_decrypt(length, (unsigned char *) cipher, (unsigned char *) *plain, key->privateKey, RSA_PKCS1_PADDING);
}


int myrsa::sign(const unsigned char *plain, int length, unsigned char **cipher, uint &r_length)
{
	key->reload();
	EVP_PKEY       *pkey = EVP_PKEY_new();

	EVP_PKEY_assign_RSA(pkey, key->privateKey);
	EVP_MD_CTX md_ctx;

	*cipher = (unsigned char *) malloc(1024 * 4);
	memset((void *) *cipher, 0, 1024 * 4);
	EVP_SignInit(&md_ctx, EVP_sha1());
	EVP_SignUpdate(&md_ctx, plain, length);
	if (EVP_SignFinal(&md_ctx, *cipher, &r_length, pkey) <= 0)
	{
		printf("EVP_Sign Error");
		return 0;
	}
	EVP_PKEY_free(pkey);
	return 1;


//    size_t templen, inlen;
//    EVP_MD_CTX ctx;
//    const EVP_MD *digest = NULL;
//    
//    unsigned char outbuf[EVP_MAX_MD_SIZE];
//
//    digest=EVP_md5();
//
//    EVP_MD_CTX_init(&ctx);
//    EVP_DigestInit(&ctx,digest);
//    if(!EVP_DigestUpdate(&ctx, content, length)) {
//        EVP_MD_CTX_cleanup(&ctx);
//        return 0;
//    }
//    if (!EVP_DigestFinal(&ctx, outbuf, &r_length)) {
//        EVP_MD_CTX_cleanup(&ctx);
//        return 0;
//    }
//    *result = (unsigned char *)malloc(r_length);
//    memcpy(*result, outbuf, r_length);
//    EVP_MD_CTX_cleanup(&ctx);
//    return r_length;

}

int myrsa::verify(const unsigned char *cipher, int length, unsigned char *plain, uint plainlen)
{
	key->reload();
	EVP_PKEY *pkey = EVP_PKEY_new();

	EVP_PKEY_assign_RSA(pkey, key->publicKey);

	EVP_MD_CTX md_ctx;

	EVP_VerifyInit(&md_ctx, EVP_sha1());
	EVP_VerifyUpdate(&md_ctx, plain, plainlen);
	if (EVP_VerifyFinal(&md_ctx, cipher, length, pkey) <= 0)
	{
		return 0;
	}
	EVP_PKEY_free(pkey);
	return 1;
}


//char *myrsa::base64encode(const unsigned char *inputBuffer, int inputLen) {
//    EVP_ENCODE_CTX  ctx;
//    int base64Len = (((inputLen+2)/3)*4) + 1; // Base64 text length
//    int pemLen = base64Len + base64Len/64; // PEM adds a newline every 64 bytes
//    char* base64 = new char[pemLen];
//    int result;
//    EVP_EncodeInit(&ctx);
//    EVP_EncodeUpdate(&ctx, (unsigned char *)base64, &result, (unsigned char *)inputBuffer, inputLen);
//    EVP_EncodeFinal(&ctx, (unsigned char *)&base64[result], &result);
//    return base64;
//}
