//
// Usage: cli IP:PORT
//
// Address: HUGUrwcFy1VC91nq7tRuZpaJqndoHDw64e
// Pubkey:  0256460f34a2e5405c899d35cbb5f71d2fe3081a07d3fcb47b6fb3797bf9e717a2
// Privkey: LWFutte4iZvna17pwecwExhQSFn1ic3EJeswcwhJMamvhqLafmgW
//
// Address: HEn5QNriNknvzee1D1YpLQaMpz9adCMoRZ
// Pubkey:  03e456e5896a4087284b169d65d1048845a7b1d45db370d499ed32cd6bcaf7a45a
// Privkey: LSehfPTP4ejs3Z1h37b2adw9yAVykiLFdKLWsiwuTXwZHsPZo5Dk
//

#include "txcommon.h"


const char *privkey = "LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm";	// privkey
const char *from_addr = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";	
const char *to_addr = "HUGUrwcFy1VC91nq7tRuZpaJqndoHDw64e";


Params_type_t _cliparams;


string	create_token();
string	send_token();


int	main(int ac, char *av[])
{
	char	svr[256] = {0};

	if (ac >= 2)
	{
		snprintf(svr, sizeof(svr), "tcp://%s", av[1]);
	}
	else
	{
		snprintf(svr, sizeof(svr), "tcp://%s:%d", "192.168.1.11", DEFAULT_CLIENT_PORT);
	}

	printf("CLIENT: connect to %s (pid=%d)\n\n", svr, getpid());

	zmq::context_t context(1);

	zmq::socket_t requester(context, ZMQ_REQ);
	requester.connect(svr);

	// Load params for sign/verify
	_cliparams = load_params("../lib/params.dat");


	string data = create_token();

	bool ret = s_send(requester, data);

	string reply = s_recv(requester);

	printf("CLIENT: Create: reply=%s  ret=%d\n", reply.c_str(), ret);


	for (int count = 0; count < 2; count++)
	{
		string data = send_token();

		bool ret = s_send(requester, data);

		string reply = s_recv(requester);

#ifdef DEBUG
		sleepms(1);
		if (count % 1000 == 0)
			printf("CLIENT: Send %7d: reply=%s  ret=%d\n",
				count + 1, reply.c_str(), ret);
#else
		if (count % 10000 == 0)
			printf("CLIENT: Send %7d: reply=%s  ret=%d\n", 
				count + 1, reply.c_str(), ret);
#endif
	}
}


//
// 토큰 생성 명령
// - TX 기록 TXID 반환
// - leveldb에 엔트리 생성 (코인 관련)
//
string	create_token()
{
	static	int	count = 0;

	xserialize hdrszr, bodyszr;
	tx_header_t	txhdr;
	tx_create_token_t txcreate;
	char		tmp[256] = {0};

	count++;

	txcreate.from_addr = from_addr;
	txcreate.to_addr = to_addr;
	txcreate.token_name = "XTOKEN";
	txcreate.quantity = 100000000;
	txcreate.smallest_unit = 0.0001;
	txcreate.native_amount = 0.0;
	txcreate.fee = 0.0;
	sprintf(tmp, "{\"Creator\": \"Hyundai-pay\", \"Manager\": \"Lee Jae Min\"}");
	txcreate.user_data = tmp;

	seriz_add(bodyszr, txcreate);

	txhdr.nodeid = getpid();	// 임시로 
	txhdr.type = TX_CREATE_TOKEN;
	txhdr.data_length = bodyszr.size();
	txhdr.from_addr = from_addr;
	txhdr.txclock = xgetclock();
	txhdr.value = count;
	txhdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), 
				&_cliparams.PrivHelper, &_cliparams.AddrHelper);
	seriz_add(hdrszr, txhdr);

	printf("CREATE_TOKEN:\n");
	printf("    Serialize: hdr  length=%ld\n", hdrszr.size());
	printf("    Serialize: body length=%ld\n", bodyszr.size());
	printf("    Address  : %s\n", from_addr);
//	printf("    Message  : \n"); bodyszr.dump(10, 1);
	printf("    Signature: %s\n", txhdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(from_addr, txhdr.signature.c_str(), 
				bodyszr.data(), bodyszr.size(), &_cliparams.AddrHelper);
	printf("    verify_check=%d\n", verify_check);
	printf("\n");

	return hdrszr.getstring() + bodyszr.getstring();
}


string	send_token()
{
	static	int	count = 0;

	xserialize hdrszr, bodyszr;
	tx_header_t	txhdr;
	tx_send_token_t	txsend;
	char		tmp[256] = {0};

	count++;

	txsend.from_addr = from_addr;
	txsend.to_addr = to_addr;
	txsend.token_name = "XTOKEN";
	txsend.amount = 1.2345;
	txsend.native_amount = 0.0;
	txsend.fee = 0.0;
	sprintf(tmp, "{\"Comment\": \"Bigdata out-sourcing\"}");
	txsend.user_data = tmp;

	seriz_add(bodyszr, txsend);

	txhdr.nodeid = getpid();	// 임시로 
	txhdr.type = TX_SEND_TOKEN;
	txhdr.data_length = bodyszr.size();
	txhdr.from_addr = from_addr;
	txhdr.txclock = xgetclock();
	txhdr.value = count;
	txhdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), 
				&_cliparams.PrivHelper, &_cliparams.AddrHelper);
	seriz_add(hdrszr, txhdr);

	printf("SEND_TOKEN:\n");
	printf("    Serialize: hdr  length=%ld\n", hdrszr.size());
	printf("    Serialize: body length=%ld\n", bodyszr.size());
	printf("    Address  : %s\n", from_addr);
//	printf("    Message  : \n"); bodyszr.dump(10, 1);
	printf("    Signature: %s\n", txhdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(from_addr, txhdr.signature.c_str(), 
				bodyszr.data(), bodyszr.size(), &_cliparams.AddrHelper);
	printf("    verify_check=%d\n", verify_check);
	printf("\n");

	return hdrszr.getstring() + bodyszr.getstring();
}
