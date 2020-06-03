//
// Common functions
//


#include "txcommon.h"


int	_debug = 1;		// debugging level
FILE	*_logfp = NULL;

keypair_t _keypair;		// keypair of this node
Params_type_t _netparams;


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


const char *get_status_name(int status)
{
	switch (status)
	{
	case 0:				return "0";
	case STAT_VERIFY_OK:		return "STAT_VERIFY_OK";
	case STAT_VERIFY_FAIL:		return "STAT_VERIFY_FAIL";

	// mempool TX 복사 
	case STAT_BCAST_TX:		return "STAT_BCAST_TX";
	case STAT_ADD_TO_MEMPOOL:	return "STAT_ADD_TO_MEMPOOL";

	// 블록 발행 전 검사 
	case STAT_VERIFY_BLOCK_REQ:	return "STAT_VERIFY_BLOCK_REQ";
	case STAT_VERIFY_BLOCK_OK:	return "STAT_VERIFY_BLOCK_OK";
	case STAT_VERIFY_BLOCK_FAIL:	return "STAT_VERIFY_BLOCK_FAIL";

	case STAT_INIT:			return "STAT_INIT";
	case STAT_VERIFY_REQUEST:	return "STAT_VERIFY_REQUEST";
	case STAT_VERI_RESULT:		return "STAT_VERI_RESULT";

	case STAT_ERROR:		return "STAT_ERROR";
	case STAT_VALID:		return "STAT_VALID";

	case FLAG_SENT_TX:		return "FLAG_SENT_TX";
	case FLAG_TX_LOCK:		return "FLAG_TX_LOCK";

	default:
		printf("Unknown status %d\n", status); return "UNKNOWN";
	}
}


const char *get_type_name(int type)
{
	switch (type)
	{
	case 0:				return "0";

	case TX_BLOCK_GEN_REQ:		return "TX_BLOCK_GEN_REQ"; 
	case TX_BLOCK_GEN_REPLY:	return "TX_BLOCK_GEN_REPLY"; 
	case TX_BLOCK_GEN:		return "TX_BLOCK_GEN"; 

	case TX_VERIFY_REPLY:		return "TX_VERIFY_REPLY"; 

        case TX_CREATE_TOKEN:		return "TX_CREATE_TOKEN"; 
        case TX_SEND_TOKEN:		return "TX_SEND_TOKEN"; 

        case TX_CREATE_CHANNEL:		return "TX_CREATE_CHANNEL"; 
        case TX_PUBLISH_CHANNEL:	return "TX_PUBLISH_CHANNEL"; 
        case TX_SUBSCRIBE_CHANNEL:	return "TX_SUBSCRIBE_CHANNEL"; 

        case TX_CREATE_CONTRACT:	return "TX_CREATE_CONTRACT"; 

        case TX_GRANT_REVOKE:		return "TX_GRANT_REVOKE"; 
        case TX_DESTROY:		return "TX_DESTROY"; 

        case TX_CREATE_WALLET:		return "TX_CREATE_WALLET"; 
        case TX_SET_WALLET:		return "TX_SET_WALLET"; 
        case TX_LIST_WALLET:		return "TX_LIST_WALLET"; 
        case TX_CREATE_KEYPAIR:		return "TX_CREATE_KEYPAIR"; 

        case TX_CREATE_ACCOUNT:		return "TX_CREATE_ACCOUNT"; 
        case TX_SET_ACCOUNT:		return "TX_SET_ACCOUNT"; 
        case TX_LIST_ACCOUNT:		return "TX_LIST_ACCOUNT"; 
	default:
		printf("Unknown type %d\n", type); return "UNKNOWN";
	}
}


int	load_params_dat(const char *path)
{
	// load params set
	if (path == NULL)
		_netparams = load_params("params.dat");
	else
		_netparams = load_params(path);

//	if (_netparams.error != 0)
	{
		_netparams.PrivHelper = privinfo;
		_netparams.AddrHelper = addrinfo;
	}

	return _netparams.error;
}


keypair_t create_keypair()
{
	keypair_t kp;

	kp = create_keypairs(&_netparams.PrivHelper, &_netparams.AddrHelper);

	printf("\n");
	printf("address : %s\n", kp.walletAddr.c_str());
	printf("pubkeyhash : %s\n", kp.pubkeyHash.c_str());
	printf("pubkey : %s\n", kp.pubkey.c_str());
	printf("privatekey : %s\n", kp.privateKey.c_str());
	printf("\n");

	return kp;
}


keypair_t create_keypair(const uchar *privkey, size_t keylen)
{
	keypair_t kp;

	assert(keylen == 32);

	bool ret = kp.secret.SetPrivKey(&privkey[0], &privkey[32], true);
	printf("SetPrivKey %s\n\n", ret ? "OK" : "FAIL");

	EccAutoInitReleaseHandler::initEcc();


	PrivateKeyHelperConstant privHelper(_netparams.PrivHelper.privateKeyPrefix, _netparams.PrivHelper.addrChecksum);
	WalletAddrHelperConstant addrHelper(_netparams.AddrHelper.pubKeyAddrPrefix, _netparams.AddrHelper.scriptAddrPrefix, _netparams.AddrHelper.addrChecksum);

	CBitcoinAddress newaddr(kp.secret.GetPubKey().GetID(), addrHelper);
	kp.privateKey = CBitcoinSecret(kp.secret, privHelper).ToString();
	kp.pubkey = HexStr(kp.secret.GetPubKey());
	kp.pubkeyHash = HexStr(kp.secret.GetPubKey().GetID());
	kp.walletAddr = newaddr.ToString();

	printf("	privkey		= %s\n", kp.privateKey.c_str());
	printf("	pubkey		= %s\n", kp.pubkey.c_str());
	printf("	pubkeyHash	= %s\n", kp.pubkeyHash.c_str());
	printf("	walletAddr	= %s\n", kp.walletAddr.c_str());

	return kp;
}



tx_header_t&	parse_header_body(txdata_t& txdata)
{
	xserialize hdrszr, bodyszr;

	hdrszr.setstring(txdata.orgdataser);	// 헤더 + 바디 모두 들어가 있음 
	deseriz(hdrszr, txdata.hdr, 0);		// 헤더 부분 먼저 deserialize
	txdata.hdr.recvclock = xgetclock();

	string body = hdrszr.getcurstring();	// 헤더 뒷부분: body
	bodyszr.setstring(body);		// body 부분 

	txdata.bodyser = body;

	xserialize tmpszr;
	seriz_add(tmpszr, txdata.hdr);
	txdata.hdrser = tmpszr.getstring();	// 헤더 serialization 교체

	return txdata.hdr;
}


string	dump_tx(const char *title, txdata_t& txdata, bool disp)
{
	char	buf[256] = {0};

	snprintf(buf, sizeof(buf), "%s%-20s %-20s / sz=%-3ld/%-4ld / valid=%d", 
		title, get_type_name(txdata.hdr.type), 
		txdata.hdr.status > 0 ? get_status_name(txdata.hdr.status) : "", 
		txdata.hdrser.size(), txdata.bodyser.size(), txdata.hdr.valid);

	if (disp)
		printf("%s\n", buf);

	string retstr = buf;
	return retstr;
}


int	logprintf(int level, ...)
{
	extern FILE	*_logfp;
	va_list		ap;

	va_start(ap, level);

	if (_logfp == NULL)
		return 0;

	char *format = va_arg(ap, char *);
	if (format == NULL)
		return -1;

	// 출력..
	if (level <= _debug)
	{
		char	*tmpbuf = NULL;

		tmpbuf = (char *) calloc(1, 16 * 1024);
		vsprintf(tmpbuf, format, ap);

		fprintf(_logfp, "%s %s", datestring().c_str(), tmpbuf);
		fprintf(stdout, "%s %s", datestring().c_str(), tmpbuf);

		free(tmpbuf);

		fflush(_logfp);
	}

	return 1;
}


string	datestring(time_t tm)
{
	struct tm *tp = NULL;

	if (tm == 0)
		tm = time(NULL);
	tp = localtime(&tm);

	char	buf[40] = {0};
	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
		tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);

	string strdate = buf;

	return strdate;
}
