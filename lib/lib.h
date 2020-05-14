//
// lib.h - Hash functions
//

#ifndef __LIB_H__
#define __LIB_H__


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <string>


using namespace std;


#include "aes.h"
#include "base58.h"
#include "base64.h"
#include "leveldb.h"
#include "md5.h"
#include "params.h"
#include "rc4.h"
#include "safe_queue.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"
#include "xserial.h"
#include "bigint.hpp"
#include "zhelpers.hpp"


// simple hash functions
string	sha1(string instr, bool tohex = true);		// C++ binary
string	sha256(string instr, bool tohex = true);	// C++ binary
string	sha512(string instr, bool tohex = true);	// C++ binary

char	*sha1(char *outhash, const char *str, const size_t len);
char	*sha256(char *outhash, const char *str, const size_t len);
char	*sha512(char *outhash, const char *str, const size_t len);
char	*md5(char *outhash, const char *str, const size_t len);

char	*bin2hex(char *hexbuf, const void *binbuf, const size_t binbuflen);
void	*hex2bin(void *binbuf, const char *hexbuf, const size_t hexbuflen);


// util.cpp
double	xgetclock();
string	bin2hex(const char *bin, const size_t binlen);
string	hex2bin(const char *hexstr, const size_t len);
string	hex2bin(const string& hexstr);


#endif	// __LIB_H__
