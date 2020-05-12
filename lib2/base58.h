#ifndef __BASE58_H
#define __BASE58_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <string>


using namespace std;


string base58_encode(string instr);	// C++ binary
string base58_decode(string instr);	// C++ binary


bool base58_encode(char *outb58, size_t *outb58sz, const void *bin, size_t binsz);
bool base58_check_encode(char *outb58c, size_t *outb58c_sz, uint8_t ver, const void *data, size_t datasz);

bool base58_decode(void *outbin, size_t *outbinsz, const char *b58, size_t b58sz);
int base58_check(const void *outbin, size_t binsz, const char *b58, size_t b58sz);


#endif	// __BASE58_H
