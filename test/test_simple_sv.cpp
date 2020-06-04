//
// Sign: 2500 / sec
// Verify: 3300 / sec
//

#include <string.h>
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/ec.h"
#include "openssl/sha.h"
#include "openssl/ecdsa.h"

#ifdef _DEBUG
#pragma comment (lib, "libcryptoMDd.lib")
#pragma comment (lib, "libsslMDd.lib")
#else
#pragma comment (lib, "libcryptoMD.lib")
#pragma comment (lib, "libsslMD.lib")
#endif


EC_KEY          *ecKey = NULL;


void	KeyGenInit()
{
	int             nidEcc;

	// Set Key Type.
	nidEcc = OBJ_txt2nid("secp256k1");
	ecKey = EC_KEY_new_by_curve_name(nidEcc);
	if (ecKey == NULL)  ERR_print_errors_fp(stderr);

	// Generate Key.
	EC_KEY_generate_key(ecKey);

}

void	KeyGenFinal()
{
	EC_KEY_free(ecKey);
}

void SignAndVerifyTest()
{
	SHA256_CTX      c;
	unsigned char   msg[SHA256_DIGEST_LENGTH];
	static unsigned char   sig[256];                   // Must greater than ECDSA_size(ecKey)
	static unsigned int    lenSig;
	static int first = 1;
	int             iRet;


	// Generate Hash for signing
	SHA256_Init(&c);
	SHA256_Update(&c, "This is Data for Signing.", 25);
	SHA256_Final(msg, &c);
	OPENSSL_cleanse(&c, sizeof(c));

//	memset(msg, 0, sizeof(msg));

	// Sign Message Digest.
	if (first)
	{
		first = 0;
		ECDSA_sign(0, msg, SHA256_DIGEST_LENGTH, sig, &lenSig, ecKey);
	}
//	iRet = ECDSA_verify(0, msg, SHA256_DIGEST_LENGTH, sig, lenSig, ecKey);
//	printf("Before Fake : Verify Result is %d \n", iRet);

	// Change Message Digest.
//	msg[0]++;
//	iRet = ECDSA_verify(0, msg, SHA256_DIGEST_LENGTH, sig, lenSig, ecKey);
//	printf("After Fake  : Verify Result is %d \n", iRet);
//	puts("\n------------------------------\n");
}


int main()
{
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	KeyGenInit();

	for (int ii = 0; ii < 10000; ii++)
	{
		SignAndVerifyTest();
	}

	ERR_print_errors_fp(stderr);

	return 0;
}
