//
//  xrsa.h
//  openssl
//
//  Created by Xinbao Dong on 15/4/5.
//  Copyright (c) 2015年 com.dongxinbao. All rights reserved.
//


#ifndef __OPENSSL__XRSA__
#define __OPENSSL__XRSA__


#include <stdio.h>
#include "rsakey.h"


typedef unsigned char uchar;
typedef unsigned int uint;


class xrsa
{
public:
	rsakey *key;
	xrsa(rsakey * key);

	// signing function: encrypt by private key and decrypt by public key
	int encrypt_block(const uchar *plain, int length, uchar **cipher, uint &r_length);	// limit 100 bytes
	int decrypt_block(const uchar *cipher, int length, uchar **plain, uint &r_length);	// limit 128 bytes
	string encrypt_string(string plaindata);	// no limit
	string decrypt_string(string encdata);		// no limit

	// enveloping function: encrypt by public key and decrypt by private key
	int evp_encrypt_block(const uchar *plain, int length, uchar **cipher, uint &r_length);	// limit 100 bytes
	int evp_decrypt_block(const uchar *cipher, int length, uchar **plain, uint &r_length);	// limit 128 bytes
	string evp_encrypt_string(string plaindata);	// no limit
	string evp_decrypt_string(string encdata);	// no limit

	int sign(const uchar *plain, int length, uchar **cipher, uint &r_length);
	int verify(const uchar *cipher, int length, uchar *plain, uint plainlen);

//	char *sign(char *content, int length);
//	char *verify(char *content, int length);
	char *base64encode(const uchar *inputBuffer, int inputLen);
};

#endif	// defined(__OPENSSL__XRSA__)
