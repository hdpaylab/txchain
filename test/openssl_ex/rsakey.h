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


class rsakey
{
public:
	rsakey()
	{
		rsa = privkey = pubkey = NULL;
	};

	RSA *rsa;
	RSA *privkey;
	RSA *pubkey;

	rsakey(string pubkeyFile, string privkeyFile);
	~rsakey();
	void generateNewKey(string pubkeyFile, string privkeyFile);
	void reload();

private:
	string privname;
	string pubname;
};


#endif /* defined(__OPENSSL__KEY__) */
