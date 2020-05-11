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


using namespace std;


int	txsend2xsz(tx_send_token_t& tx, xserial& xsz);


int	main(int ac, char *av[])
{
	const char ESC = TX_DELIM;
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
	txsend.native_amount = 0;
	txsend.fee = 0;
	txsend.user_data = "TEST SEND TOKEN";

	xserial xsz(4 * 1024);

	txsend2xsz(txsend, xsz);

	tx_sign_t sign;
	sign.signature = sign_message_bin(privkey, xsz.getbuf(), xsz.getsize(), &params.PrivHelper, &params.AddrHelper);
	sign.sign_clock = xgetclock();

	printf("address  : %s\n", from_addr);
	printf("message  : "); xsz.dump();
	printf("signature: %s\n", sign.signature.c_str());

	int verify_check = verify_message_bin(from_addr, sign.signature.c_str(), xsz.getbuf(), xsz.getsize(), &params.AddrHelper);
	printf("verify_check=%d\n", verify_check);
	printf("\n");

	char	data[1024];

	for (int count = 0; count < 100000; count++)
	{
		txdata_t txdata;

		txdata.data = data;
		txdata.seq = count + 1;
		txdata.valid = TXCHAIN_STATUS_VALID;
		txdata.verified = TXCHAIN_STATUS_EMPTY;
		txdata.status = TXCHAIN_STATUS_EMPTY;

		bool ret = s_send(requester, txdata.data);

		string reply = s_recv(requester);
		if (count % 10000 == 0)
			cout << "Client send count=" << count << " reply=" << reply << endl;

		if (txdata.seq == MAX_SEQ) break;

#ifdef DEBUG
		sleepms(DEBUG_SLEEP_MS);
#else
		if (count % 10000 == 0)
#endif
			printf("CLIENT: Send %7d\n", count + 1);
	}
}


int	txsend2xsz(tx_send_token_t& txsend, xserial& xsz)
{
	xsz << txsend.type;
	xsz << txsend.seq;
	xsz << txsend.from_addr;
	xsz << txsend.to_addr;
	xsz << txsend.token_name;
	xsz << txsend.amount;
	xsz << txsend.native_amount;
	xsz << txsend.fee;
	xsz << txsend.user_data;

	return 1;
}
