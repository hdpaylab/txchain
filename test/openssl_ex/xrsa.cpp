//
//  xrsa.cpp
//  openssl
//
//  Created by Xinbao Dong on 15/4/5.
//  Copyright (c) 2015年 com.dongxinbao. All rights reserved.
//

#include "xrsa.h"
#include <string.h>
#include <openssl/evp.h>


xrsa::xrsa(rsakey * key)
{
	this->key = key;
}


int xrsa::encrypt_block(const unsigned char *plain, int length, unsigned char **cipher, uint &r_length)
{
	key->reload();
	r_length = RSA_size(key->privkey);
	*cipher = (unsigned char *) malloc(sizeof(char) * r_length);
	memset((void *) *cipher, 0, r_length);
	int res = RSA_private_encrypt(length, (unsigned char *) plain, (unsigned char *) *cipher, key->privkey, RSA_PKCS1_PADDING);

	return res;
}


int xrsa::decrypt_block(const unsigned char *cipher, int length, unsigned char **plain, uint &r_length)
{
	key->reload();
	r_length = RSA_size(key->pubkey);
	*plain = (unsigned char *) malloc(sizeof(char) * r_length);
	memset((void *) *plain, 0, r_length);
	return RSA_public_decrypt(length, (unsigned char *) cipher, (unsigned char *) *plain, key->pubkey, RSA_PKCS1_PADDING);
}


// encryption by privkey
string xrsa::encrypt_string(string plaindata)
{
	string	encdata;
	int	ii = 0;
	uchar	*encbuf = NULL;
	uint	encbufsz = 0;

	while (ii < plaindata.size())
	{
		if (this->encrypt_block((uchar *) (plaindata.data() + ii), 100, (uchar **) &encbuf, encbufsz) <= 0)
		{
			perror("Signed File encrypt error!");
			return string();
		}
		string curdata((char *)encbuf, encbufsz);
		encdata += curdata;

		free(encbuf);
		ii += 100;	//100은 128-11보다 작으면 암호화 단위입니다.
	}

	return encdata;
}


// decryption by pubkey
string xrsa::decrypt_string(string encdata)
{
	string	decdata;
	int	ii = 0;
	char	*tt = NULL;
	unsigned int len2 = 0, wbytes = 0;

	while (ii < encdata.size())
	{
		int yy = this->decrypt_block((uchar *) encdata.data() + ii, 128, (uchar **) &tt, len2);

		// remove space
		if (ii + 128 >= encdata.size())
		{
			yy = 100;
			while (tt[yy - 1] == 0)
			{
				yy--;
				if (yy == 0)
				{
					break;
				}
			}
		}
		string curdata(tt, yy);
		decdata += curdata;

		free(tt);
		ii += 128;
	}

	return decdata;
}


int xrsa::evp_encrypt_block(const unsigned char *plain, int length, unsigned char **cipher, uint &r_length)
{
	key->reload();
	r_length = RSA_size(key->pubkey);
	*cipher = (unsigned char *) malloc(sizeof(char) * r_length);
	memset((void *) *cipher, 0, r_length);
	int res = RSA_public_encrypt(length, (unsigned char *) plain, (unsigned char *) *cipher, key->pubkey, RSA_PKCS1_PADDING);

	return res;
}


int xrsa::evp_decrypt_block(const unsigned char *cipher, int length, unsigned char **plain, uint &r_length)
{
	key->reload();
	r_length = RSA_size(key->privkey);
	*plain = (unsigned char *) malloc(sizeof(char) * r_length);
	memset((void *) *plain, 0, r_length);
	return RSA_private_decrypt(length, (unsigned char *) cipher, (unsigned char *) *plain, key->privkey, RSA_PKCS1_PADDING);
}


// encryption by pubkey
string xrsa::evp_encrypt_string(string plaindata)
{
	string	encdata;
	int	ii = 0;
	uchar	*encbuf = NULL;
	uint	encbufsz = 0;

	while (ii < plaindata.size())
	{
		if (this->evp_encrypt_block((uchar *) (plaindata.data() + ii), 100, (uchar **) &encbuf, encbufsz) <= 0)
		{
			perror("Signed File encrypt error!");
			return string();
		}
		string curdata((char *)encbuf, encbufsz);
		encdata += curdata;

		free(encbuf);
		ii += 100;	//100은 128-11보다 작으면 암호화 단위입니다.
	}

	return encdata;
}


// decryption by privkey
string xrsa::evp_decrypt_string(string encdata)
{
	string	decdata;
	int	ii = 0;
	char	*tt = NULL;
	unsigned int len2 = 0, wbytes = 0;

	while (ii < encdata.size())
	{
		int yy = this->evp_decrypt_block((uchar *) encdata.data() + ii, 128, (uchar **) &tt, len2);

		// remove space
		if (ii + 128 >= encdata.size())
		{
			yy = 100;
			while (tt[yy - 1] == 0)
			{
				yy--;
				if (yy == 0)
				{
					break;
				}
			}
		}
		string curdata(tt, yy);
		decdata += curdata;

		free(tt);
		ii += 128;
	}

	return decdata;
}


int xrsa::sign(const unsigned char *plain, int length, unsigned char **cipher, uint &r_length)
{
	key->reload();
	EVP_PKEY       *pkey = EVP_PKEY_new();

	EVP_PKEY_assign_RSA(pkey, key->privkey);
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

int xrsa::verify(const unsigned char *cipher, int length, unsigned char *plain, uint plainlen)
{
	key->reload();
	EVP_PKEY *pkey = EVP_PKEY_new();

	EVP_PKEY_assign_RSA(pkey, key->pubkey);

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


char *xrsa::base64encode(const unsigned char *inputBuffer, int inputLen)
{
	EVP_ENCODE_CTX  ctx;

	int base64Len = (((inputLen+2)/3)*4) + 1; // Base64 text length
	int pemLen = base64Len + base64Len/64; // PEM adds a newline every 64 bytes
	char* base64 = new char[pemLen];
	int result;
	EVP_EncodeInit(&ctx);
	EVP_EncodeUpdate(&ctx, (unsigned char *)base64, &result, (unsigned char *)inputBuffer, inputLen);
	EVP_EncodeFinal(&ctx, (unsigned char *)&base64[result], &result);

	return base64;
}
