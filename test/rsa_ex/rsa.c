#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <string.h>
#include <unistd.h>
#include "rsa.h"

int	main(int ac, char *av[])
{
	RSA *rsa = NULL;
	RSA *pubkey = NULL;

	unsigned char sign_text[256] = {0};
	unsigned char decrypt_text[256] = {0};
	unsigned char data[256] = 
		"댁 RSA媛  吏 ㅽ명怨 듬 명媛  댁.";

	unsigned int num = 0;

	size_t pri_len = 0;
	size_t pub_len = 0;
	char *pri_key;
	char *pub_key;
	char public_key[4096] = {0};

	printf("signed text=%s\n\n", data);

	rsa = RSA_generate_key(1024, RSA_F4, NULL, NULL);

	// make pri key
	BIO *pri = BIO_new(BIO_s_mem());
	PEM_write_bio_RSAPrivateKey(pri, rsa, NULL, NULL, 0, NULL, NULL);
	pri_len = BIO_pending(pri);
	pri_key = (char*)malloc(pri_len + 1);
	BIO_read(pri, pri_key, pri_len);
	pri_key[pri_len] = '\0';

	printf("PRIVKLEY=%s\n", pri_key);

	// make pub key
	BIO *pub = BIO_new(BIO_s_mem());
	PEM_write_bio_RSA_PUBKEY(pub, rsa);		
	pub_len = BIO_pending(pub);
	pub_key = (char*)malloc(pub_len + 1);
	BIO_read(pub, pub_key, pub_len);
	pub_key[pub_len] = '\0';

	printf("PUBKEY=%s\n", pub_key);

	printf("done.\n\n");

	// sign ready
	memset(sign_text, 0, sizeof(sign_text));
	memset(decrypt_text, 0, sizeof(decrypt_text));
	strcpy(public_key, pub_key);

	// sign pub key make
	BIO *bio = BIO_new_mem_buf((void *)pub_key, -1);
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	pubkey = PEM_read_bio_RSA_PUBKEY(bio, &pubkey, NULL, NULL);
	if (pubkey == NULL) {
		printf("ERROR: %s\n", ERR_error_string(ERR_get_error(), NULL));
	}
	BIO_free(bio);

	// sign and decrypt
	num = RSA_public_encrypt(strlen(data), data, 
			sign_text, pubkey, RSA_PKCS1_PADDING);
	printf("\nnum=%d encrypt=\n", num); 
	print_n(sign_text, sizeof(sign_text)); printf("\n");

	num = RSA_private_decrypt(num, sign_text, decrypt_text, 
					rsa, RSA_PKCS1_PADDING);
	printf("\nnum=%d decrypt=%s\n", num, decrypt_text);


	BIO_free_all(pub);
	BIO_free_all(pri);
	free(pri_key);
	free(pub_key);
	RSA_free(rsa);

	return 0;
}

