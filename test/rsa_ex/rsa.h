#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <string.h>
#include <unistd.h>


typedef unsigned char uint8_t;


void print_n(const void *_data, size_t len);

RSA *createRSA(unsigned char * key,int public);
int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted);

unsigned long long rdtsc(void);
double	xgetclock();
uint8_t	*getrandom(uint8_t *buf, int buflen);
