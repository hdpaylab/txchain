#include <vector>
#include <string>
#include <stdint.h>
#include <keys/hs_keys_wrapper.h>
#include <keys/key.h>
#include <keys/eccautoinitreleasehandler.h>
#include <keys/bitcoinaddress.h>
#include <utils/utilstrencodings.h>
#include <stdio.h>
#include <stdlib.h>


typedef unsigned char	uchar;


// private-key-version = 83                # Version bytes used for exporting private keys.
// address-checksum-value = 48444143       # Bytes used for XOR in address checksum calculation.
struct PrivateKeyHelpInfo privinfo = {
//      "8075fa23", "cb507245"
        "83", "48444143"
};

// address-pubkeyhash-version = 28         # Version bytes used for pay-to-pubkeyhash addresses.
// address-scripthash-version = 08         # Version bytes used for pay-to-scripthash addresses.
// address-checksum-value = 48444143       # Bytes used for XOR in address checksum calculation.
struct WalletAddrHelpInfo addrinfo = {
//      "003fd61c", "0571a3e6", "cb507245"
        "28", "08", "48444143"
};


void	test_create_key_pairs(void)
{
	keypairs_type_t *keypairs = create_key_pairs(&privinfo, &addrinfo);

	printf("address : %s\n", keypairs->walletaddr);
	printf("\n");

	printf("pubkeyhash : %s\n", keypairs->pubkeyhash);
	printf("\n");

	printf("pubkey : %s\n", keypairs->pubkey);
	printf("\n");

	printf("privatekey : %s\n", keypairs->privatekey);
	printf("\n");

	free(keypairs);
}

void	test_create_pub_key_binarys(void)
{
	unsigned char *keypairs = create_pub_key_binarys(&privinfo, &addrinfo);

	printf("pub key hex : ");	
	dumpbin((char *)keypairs, 66);

	free(keypairs);
}


int	main()
{
	test_create_key_pairs();

	test_create_pub_key_binarys();

	return 0;
}
