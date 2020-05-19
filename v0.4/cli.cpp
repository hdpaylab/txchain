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


int	main(int ac, char *av[])
{
	char	svr[256] = {0}, tmp[256] = {0};

	if (ac >= 2)
	{
		snprintf(svr, sizeof(svr), "tcp://%s", av[1]);
	}
	else
	{
		snprintf(svr, sizeof(svr), "tcp://%s:%d", "192.168.1.10", DEFAULT_CLIENT_PORT);
	}

	printf("CLIENT: connect to %s (pid=%d)\n\n", svr, getpid());

	zmq::context_t context(1);

	zmq::socket_t requester(context, ZMQ_REQ);
	requester.connect(svr);

	// Load params for sign/verify
	Params_type_t params = paramsget("../lib/params.dat");


	const char *privkey = "LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm";	// privkey
	const char *from_addr = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";	
	const char *to_addr = "HUGUrwcFy1VC91nq7tRuZpaJqndoHDw64e";

	for (int count = 0; count < 1000000; count++)
	{
		xserialize hdrszr, bodyszr;
		tx_header_t	txhdr;
		tx_send_token_t	txsend;

		txsend.from_addr = from_addr;
		txsend.to_addr = to_addr;
		txsend.token_name = "XTOKEN";
		txsend.amount = count + 0.12345;
		txsend.native_amount = 0.0;
		txsend.fee = 0.0;
		sprintf(tmp, "Send test: %d", count + 1);
		txsend.user_data = tmp;
		txsend.sign_clock = xgetclock();

		seriz_add(bodyszr, txsend);

		txhdr.nodeid = getpid();	// 임시로 
		txhdr.type = TX_SEND_TOKEN;
		txhdr.status = 0;
		txhdr.data_length = bodyszr.size();
		txhdr.valid = -1;
		txhdr.txid = "";
		txhdr.from_addr = from_addr;
		txhdr.txclock = xgetclock();
		txhdr.flag = 0;
		txhdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), &params.PrivHelper, &params.AddrHelper);
		seriz_add(hdrszr, txhdr);


		printf("Serialize: body length=%ld\n", bodyszr.size());
//		printf("address  : %s\n", from_addr);
//		printf("message  : \n"); bodyszr.dump(10, 1);
		printf("signature: %s\n", txhdr.signature.c_str());

		// 발송 전에 미리 검증 테스트 
		int verify_check = verify_message_bin(from_addr, txhdr.signature.c_str(), bodyszr.data(), bodyszr.size(), &params.AddrHelper);
		printf("verify_check=%d\n", verify_check);
		printf("\n");

		bool ret = s_send(requester, hdrszr.getstring() + bodyszr.getstring());

		string reply = s_recv(requester);

#ifdef DEBUG
		sleep(1);
#else
		if (count % 10000 == 0)
#endif
			printf("CLIENT: Send %7d: reply=%s\n", count + 1, reply.c_str());
	}
}
