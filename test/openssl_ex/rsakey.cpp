//
//  rsakey.cpp
//  openssl
//
//  Created by Xinbao Dong on 15/4/4.
//  Copyright (c) 2015年 com.dongxinbao. All rights reserved.
//


#define LENGTH 1024


#include "rsakey.h"
#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>


rsakey::rsakey(string pubkeyFile, string privkeyFile)
{
	rsa = NULL;
	privkey = NULL;
	pubkey = NULL;
	privname = privkeyFile;
	pubname = pubkeyFile;

	if (!privkeyFile.empty())
	{
		FILE *fp = fopen(privkeyFile.c_str(), "r");

		if (fp == NULL)
		{
			cout << "Private Key File Error!" << endl;
			return;
		}
		privkey = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
		fclose(fp);
	}
	if (!pubkeyFile.empty())
	{
		FILE *fp = fopen(pubkeyFile.c_str(), "r");

		if (fp == NULL)
		{
			cout << "Public Key File Error!" << endl;
			return;
		}
//		rsa = PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
		pubkey = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
		fclose(fp);
		return;
	}
	cout << "Error Open Private Key or Public Key!" << endl;
}

void rsakey::reload()
{
	if (!privname.empty())
	{
		FILE *fp = fopen(privname.c_str(), "r");

		if (fp == NULL)
		{
			cout << "Private Key File Error!" << endl;
			return;
		}
		privkey = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
		fclose(fp);
	}
	if (!pubname.empty())
	{
		FILE *fp = fopen(pubname.c_str(), "r");

		if (fp == NULL)
		{
			cout << "Public Key File Error!" << endl;
			return;
		}
		pubkey = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
		fclose(fp);
		return;
	}
}

void rsakey::generateNewKey(string pubkeyFile, string privkeyFile)
{
	privname = privkeyFile;
	pubname = pubkeyFile;

	RSA *rsa = RSA_generate_key(LENGTH, RSA_F4, NULL, NULL);

	if (rsa == NULL)
	{
		cout << "RSA_generate_key Error!" << endl;
		return;
	}

	BIO *priBio = BIO_new_file(privkeyFile.c_str(), "w");

	if (PEM_write_bio_RSAPrivateKey(priBio, rsa, NULL, NULL, 0, NULL, NULL) <= 0)
	{
		cout << "Save to private key file error!" << endl;
	}

	BIO *pubBio = BIO_new_file(pubkeyFile.c_str(), "w");

	if (PEM_write_bio_RSAPublicKey(pubBio, rsa) <= 0)
	{
		cout << "Save to public key file error!" << endl;
	}

	BIO_free(priBio);
	BIO_free(pubBio);

//    FILE *priFile = fopen(privkeyFile.c_str(), "w+");
//    if (PEM_write_RSAPrivateKey(priFile, rsa, EVP_des_ede3_ofb(), NULL, NULL, NULL, NULL) <= 0) {
//        cout << "Save to private key file error!" << endl;
//    }
//    fclose(priFile);
//    
//    FILE *pubFile = fopen(pubkeyFile.c_str(), "w+");
//    if (PEM_write_RSA_PUBKEY(pubFile, rsa) <= 0) {
//        cout << "Save to public key file error!" << endl;
//    }
//    fclose(pubFile);
//    
//    BIO *bp = BIO_new(BIO_s_file());
//    if (bp == NULL) {
//        cout << "BIO_new Error!" << endl;
//        return ;
//    }
//    if (BIO_write_filename(bp, (void *)pubkeyFile.c_str()) <= 0) {
//        cout << "BIO_write_filename Error!" << endl;
//        return ;
//    }
//    if (PEM_write_bio_RSAPublicKey(bp, rsa) != 1) {
//        cout << "PEM_write_bio_RSAPublicKey Error!" << endl;
//        return ;
//    }
//    cout << "Created Public Key" << endl;
//    
//    bp = BIO_new_file(privkeyFile.c_str(), "w+");
//    if(NULL == bp) {
//        cout << "BIO_new_file error(private key)!" << endl;
//        return ;
//    }
//    if (PEM_write_bio_RSAPrivateKey(bp, rsa, EVP_des_ede3_ofb(), NULL, NULL, NULL, NULL) != 1) {
//        cout << "PEM_write_bio_RSAPrivateKey Error!" << endl;
//        return ;
//    }
//
//    BIO_free_all(bp);
	this->rsa = rsa;
	privkey = RSAPrivateKey_dup(rsa);
	pubkey = RSAPublicKey_dup(rsa);
}

rsakey::~rsakey()
{
	if (rsa != NULL)
		RSA_free(rsa);
	if (privkey != NULL)
		RSA_free(privkey);
	if (pubkey != NULL)
		RSA_free(pubkey);
}
