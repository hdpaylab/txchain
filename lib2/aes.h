#ifndef __AES_H
#define __AES_H


#include <stdint.h>


// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// CBC enables AES encryption in CBC-mode of operation.
// CTR enables encryption in counter-mode.
// ECB enables the basic ECB 16-byte block algorithm. All can be enabled simultaneously.

// The #ifndef-guard allows it to be configured before #include'ing or at compile time.
enum {
	AES_CBC	= 1,	// AES encryption in CBC-mode of operation
	AES_CTR,	// encryption in counter-mode
	AES_ECB		// basic ECB 16-byte block algorithm. All can be enabled simultaneously
};


#define AES_BLOCKLEN	16		// Block length in bytes AES is 128bit block only


typedef struct {
	int	bits;			// 0 == 128, 192, 256
	int	type;			// 0 == AES_CBC / AES_CTR / AES_ECB

	uint8_t roundkey[256];		// 256bits==240 / 192bits==208 / 128bits==176 
	uint8_t iv[AES_BLOCKLEN];	// for CBC & CTR 

	int	keylen;			//
	int	keyexpsize;		// 
	int	nb, nk, nr;		// local
} aes_t;


aes_t	*aes_init(aes_t *aesp, const int type, const int bits, const uint8_t *key, const uint8_t *iv);
void	aes_exit(aes_t *aesp);
aes_t	*aes_set_key_iv(aes_t *aesp, const uint8_t *key, const uint8_t *iv);

// buffer size is exactly AES_BLOCKLEN bytes; buffer size MUST be mutile of AES_BLOCKLEN;
void	aes_encrypt(aes_t *aesp, uint8_t *buf, size_t buflen);
void	aes_decrypt(aes_t *aesp, uint8_t *buf, size_t buflen);


#endif	// __AES_H
