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


void SignAndVerifyTest()
{
	SHA256_CTX      c;
	EC_KEY          *ecKey = NULL;
	int             nidEcc;
	unsigned char   m[SHA256_DIGEST_LENGTH];
	unsigned char   sig[256];                   // Must greater than ECDSA_size(ecKey)
	unsigned int    lenSig;
	int             iRet;


	// Generate Hash for signing
	SHA256_Init(&c);
	SHA256_Update(&c, "This is Data for Signing.", 25);
	SHA256_Final(m, &c);
	OPENSSL_cleanse(&c, sizeof(c));

	// Set Key Type.
	nidEcc = OBJ_txt2nid("secp256k1");
	ecKey = EC_KEY_new_by_curve_name(nidEcc);
	if (ecKey == NULL)  ERR_print_errors_fp(stderr);

	// Generate Key.
	EC_KEY_generate_key(ecKey);

	// Sign Message Digest.
	ECDSA_sign(0, m, SHA256_DIGEST_LENGTH, sig, &lenSig, ecKey);
	iRet = ECDSA_verify(0, m, SHA256_DIGEST_LENGTH, sig, lenSig, ecKey);
	printf("Before Fake : Verify Result is %d \n", iRet);

	// Change Message Digest.
	m[0]++;
	iRet = ECDSA_verify(0, m, SHA256_DIGEST_LENGTH, sig, lenSig, ecKey);
	printf("After Fake  : Verify Result is %d \n", iRet);
	puts("\n------------------------------\n");

	EC_KEY_free(ecKey);
}


int main()
{
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	SignAndVerifyTest();

	ERR_print_errors_fp(stderr);

	return 0;
}
