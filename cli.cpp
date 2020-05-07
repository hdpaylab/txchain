//
// cli IP:PORT
//

#include "txcommon.h"


using namespace std;


void	req_rep();
void	push_pull(int direc);


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


	const char *privkey = "LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm";	// privkey
	const char *pubkey = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";	// pubkey

	const char *message = "Hdac Technology, 잘 가는지 검사하는 것임23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789";

	// params set
	Params_type_t params = paramsget("params.dat");

	txmsg_t	txmsg;
	txmsg.message = strdup(message);
	txmsg.pubkey = strdup(pubkey);
	txmsg.signature = sign_message(privkey, txmsg.message, &params.PrivHelper, &params.AddrHelper);

	printf("pubkey   : [%s]\n", txmsg.pubkey);
	printf("message  : [%s]\n", txmsg.message);
	printf("signature: [%s]\n", txmsg.signature);

	int verify_check = verify_message(txmsg.pubkey, txmsg.signature, txmsg.message, &params.AddrHelper);
	printf("verify_check=%d\n", verify_check);
	printf("\n");

	char	data[1024];

	for (int count = 0; count < 1000000; count++)
	{
		txdata_t txdata;

		snprintf(data, sizeof(data), "CLI-%7d %c%s%c%s%c%s", 
			count, ESC, txmsg.pubkey, ESC, txmsg.message, ESC, txmsg.signature);

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
		sleepms(DEBUG_SLEEP);
	//	if (count % 10 == 0)
#else
		if (count % 10000 == 0)
#endif
			printf("CLIENT: Send %7d\n", count);
	}
}
