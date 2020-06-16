//
//  key.h
//  openssl
//
//  Created by Xinbao Dong on 15/4/4.
//  Copyright (c) 2015年 com.dongxinbao. All rights reserved.
//

#ifndef __OPENSSL__KEY__
#define __OPENSSL__KEY__


#include <stdio.h>
#include <string>
#include <openssl/rsa.h>

using namespace std;


class Key
{
public:
	Key()
	{
		rsa = privateKey = publicKey = NULL;
	};

	RSA *rsa;
	RSA *privateKey;
	RSA *publicKey;

	Key(string publicKeyFile, string privateKeyFile);
	~Key();
	void generateNewKey(string publicKeyFile, string privateKeyFile);
	void reload();

private:
	string priName;
	string pubName;
};


#endif /* defined(__OPENSSL__KEY__) */
