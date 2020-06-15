//
// Hdac library sign verification speed:
//
// @i9900 6 core: sign=6000 TPS verify=15000 TPS
//


#include "lib.h"
#include <keys/hs_keys.h>		// KeyPairs
#include <keys/hs_keys_wrapper.h>	// xparams.h
#include <keys/key.h>


const char *_privkey = "LRU9TP4iRbnJdrtzBrmw7C1u5i2njtGKLsLShjdw73FNW4oXM2Bj";
const char *_addr = "HBRPK7cf9LKTP5SyWrGbyKd1bVknSpzD8m";	


Params_type_t _netparams;


int	main(int ac, char *av[])
{
	// Load params for sign/verify
	_netparams = load_params("../lib/params.dat");

	char	body[1024] = {0};
	string signature;

	for (int nn = 0; nn < 100000; nn++)
	{
		signature = sign_message_bin(_privkey, body, sizeof(body),
				&_netparams.PrivHelper, &_netparams.AddrHelper);

		if (nn % 10000 == 0)
		{
			printf("    Address  : %s\n", _addr);
			printf("    Signature: %s\n", signature.c_str());
		}

		// 발송 전에 미리 검증 테스트 
		int verify_check = verify_message_bin(_addr, signature.c_str(), 
					body, sizeof(body), &_netparams.AddrHelper);
		if (nn % 10000 == 0)
		{
			printf("    nn=%d verify_check=%d\n", nn, verify_check);
		}
	}

	printf("\n");

	return 0;
}
