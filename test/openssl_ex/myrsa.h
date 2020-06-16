//
//  myrsa.h
//  openssl
//
//  Created by Xinbao Dong on 15/4/5.
//  Copyright (c) 2015年 com.dongxinbao. All rights reserved.
//


#ifndef __OPENSSL__MYRSA__
#define __OPENSSL__MYRSA__


#include <stdio.h>
#include "key.h"


typedef unsigned char uchar;
typedef unsigned int uint;


class myrsa
{
public:
	Key *key;
	myrsa(Key * key);

	// signing function: encrypt by private key and decrypt by public key
	int encrypt(const uchar *plain, int length, uchar **cipher, uint &r_length);
	int decrypt(const uchar *cipher, int length, uchar **plain, uint &r_length);

	// enveloping function: encrypt by public key and decrypt by private key
	int evp_encrypt(const uchar *plain, int length, uchar **cipher, uint &r_length);
	int evp_decrypt(const uchar *cipher, int length, uchar **plain, uint &r_length);

	int sign(const uchar *plain, int length, uchar **cipher, uint &r_length);
	int verify(const uchar *cipher, int length, uchar *plain, uint plainlen);

//	char *sign(char *content, int length);
//	char *verify(char *content, int length);
//	char *base64encode(const uchar *inputBuffer, int inputLen);
};

#endif	// defined(__OPENSSL__MYRSA__)
