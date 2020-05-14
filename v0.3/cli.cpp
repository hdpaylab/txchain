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
	char	svr[256] = {0};

	if (ac >= 2)
	{
		snprintf(svr, sizeof(svr), "tcp://%s", av[1]);
	}
	else
	{
		snprintf(svr, sizeof(svr), "tcp://%s:%d", "192.168.1.10", DEFAULT_CLIENT_PORT);
	}

	printf("CLIENT: connect to %s\n\n", svr);

	zmq::context_t context(1);

	zmq::socket_t requester(context, ZMQ_REQ);
	requester.connect(svr);

	// params set
	Params_type_t params = paramsget("../lib/params.dat");


	const char *privkey = "LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm";	// privkey
	const char *from_addr = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";	
	const char *to_addr = "HUGUrwcFy1VC91nq7tRuZpaJqndoHDw64e";

	tx_send_token_t	txsend;

	txsend.type = TX_SEND_TOKEN;
	txsend.seq = 1;
	txsend.from_addr = from_addr;
	txsend.to_addr = to_addr;
	txsend.token_name = "XTOKEN";
	txsend.amount = 123.456;
	txsend.native_amount = 0.0;
	txsend.fee = 0.0;
	txsend.user_data = "TEST SEND TOKEN";
	txsend.sign_clock = xgetclock();

	xserial txsz(1 * 1024);

	seriz_add(txsz, txsend);

	tx_sign_t sign;
	sign.data_length = txsz.getsize();
	sign.signature = sign_message_bin(privkey, txsz.getdata(), txsz.getsize(), &params.PrivHelper, &params.AddrHelper);
	printf("Serialize: length=%ld\n", txsz.getsize());
	printf("address  : %s\n", from_addr);
//	printf("message  : \n"); txsz.dump(10, 1);
	printf("signature: %s\n", sign.signature.c_str());

	int verify_check = verify_message_bin(from_addr, sign.signature.c_str(), txsz.getdata(), txsz.getsize(), &params.AddrHelper);
	printf("verify_check=%d\n", verify_check);
	printf("\n");

	// sign은 verify 테스트 전에 serialize하면 안됨..
	seriz_add(txsz, sign);

	for (int count = 0; count < 100000; count++)
	{
		bool ret = s_send(requester, txsz.getstring());

		string reply = s_recv(requester);

#ifdef DEBUG
		sleep(10);
#else
		if (count % 10000 == 0)
#endif
			printf("CLIENT: Send %7d: reply=%s\n", count + 1, reply.c_str());
	}
}
