#include "keypair.h"


typedef unsigned char	uchar;


static	void	dumpbin(const char *name, uchar *buf, size_t bufsz)
{
	printf("DUMP %s(%ld):	", name, bufsz);
	for (size_t ii = 0; ii < bufsz; ii++)
	{
		printf("%02X ", buf[ii] & 0x00FF);
	}
	printf("\n");
}


////////////////////////////////////////////////////////////////////////////////


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


int	main(int ac, char *av[])
{
	bool result;

	RpcClient client{"192.168.1.10", 28822, "hdacrpc", "hdac1234", "phdac"};
	KeysHelperWithRpc helper(client);

	CKey secret;
	//secret.MakeNewKey(fCompressed);
	secret.MakeNewKey(true);

	EccAutoInitReleaseHandler::initEcc();
	CPubKey pubkey = secret.GetPubKey();

	uchar pubkeybuf[66] = {0};
	memcpy(pubkeybuf, (char *)pubkey.begin(), pubkey.size());
	dumpbin("PubKey", pubkeybuf, pubkey.size());

	uchar privkeybuf[32] = {0};
	memcpy(privkeybuf, (char *)secret.begin(), secret.size());
	dumpbin("PrivKey", privkeybuf, secret.size());

	CBitcoinAddress addr(pubkey.GetID(), helper.addrHelper());
	string privateKeyStr = CBitcoinSecret(secret, helper.privHelper()).ToString();
	string pubkeyStr = HexStr(pubkey);
	string pubkeyHashStr = HexStr(pubkey.GetID());
	string walletAddrStr = addr.ToString();

	cout << endl;
	cout << "address : " << walletAddrStr << endl;
	cout << "pubkey ID: " << pubkeyHashStr << endl;
	cout << "pubKey: " << pubkeyStr << endl;
	cout << "privkey: " << privateKeyStr << endl;

	printf("\n================================================================================\n\n");

	CKey newsecret;

	CPrivKey privkey = secret.GetPrivKey();

	result = newsecret.SetPrivKey(privkey, true);
	printf("SerPrivKey=%s\n", result ? "OK" : "FAIL");

	result = newsecret.SetPrivKey(&privkeybuf[0], &privkeybuf[32], true);
	printf("SerPrivKey=%s\n", result ? "OK" : "FAIL");

	EccAutoInitReleaseHandler::initEcc();		// 반드시 해야 함 
	CPubKey newpubkey = newsecret.GetPubKey();

	uchar newpubkeybuf[66] = {0};
	memcpy(newpubkeybuf, (char *)newpubkey.begin(), newpubkey.size());
	dumpbin("PubKey", newpubkeybuf, newpubkey.size());

	uchar newprivkeybuf[32] = {0};
	memcpy(newprivkeybuf, (char *)newsecret.begin(), newsecret.size());
	dumpbin("PrivKey", newprivkeybuf, newsecret.size());

	CBitcoinAddress newaddr(pubkey.GetID(), helper.addrHelper());
	string newprivateKeyStr = CBitcoinSecret(newsecret, helper.privHelper()).ToString();
	string newpubkeyStr = HexStr(newpubkey);
	string newpubkeyHashStr = HexStr(newpubkey.GetID());
	string newwalletAddrStr = newaddr.ToString();

	cout << endl;
	cout << "address : " << newwalletAddrStr << endl;
	cout << "pubkey ID: " << newpubkeyHashStr << endl;
	cout << "pubKey: " << newpubkeyStr << endl;
	cout << "privkey: " << newprivateKeyStr << endl;

	return 0;
}
