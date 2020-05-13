#ifndef __BASE64_H
#define __BASE64_H


#include <iostream>
#include <cstring>
#include <string>
#include <vector>


using namespace std;


vector<unsigned char> base64_decode(const char* p, bool* pfInvalid = NULL);
string base64_decode(const string& str);
string base64_encode(const unsigned char* pch, size_t len);
string base64_encode(const string& str);


#endif	// __BASE64_H
