#include "txcommon.h"


//
// Publisher - Sending TX to other nodes
//
void	*thread_publisher(void *info_p)
{
        int	sendport = *(int *)info_p;
	int	count = 0;
	double	tmstart = 0, tmend = 0;
	char	bindstr[100] = {0};


	printf("thread_publisher: sendport=%d START!\n", sendport);

	// ZMQ setup: prepare our context and publisher
	zmq::context_t context_pub(1);
	zmq::socket_t xpub(context_pub, ZMQ_PUB);
	sprintf(bindstr, "tcp://*:%d", sendport);
	xpub.bind(bindstr);

	int bufsize = 1 * 1024 * 1024;	// 1MB ???? 
	xpub.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

	// Avoiding message loss
	int one = 1;
	xpub.setsockopt(ZMQ_XPUB_NODROP, &one, sizeof(one));

	// max send message length
	int qsize = 10000;		// 10000 ??
	xpub.setsockopt(ZMQ_SNDHWM, &qsize, sizeof(qsize));

	sleepms(500);

	tmstart = xgetclock();

	while (1)
	{
		txdata_t txdata;

		count++;
		txdata = _sendq.pop();

		printf("thread_publisher: Broadcast %s: %s\n", 
			get_status_name(txdata.status), txdata.data.c_str());

		string senddata;

		senddata = string(ZMQ_FILTER) + txdata.data;
//		printf("SEND DUMP(%ld): ", senddata.length()); 
//		dumpbin(senddata.c_str(), senddata.length());

		bool ret = s_send(xpub, senddata);

		if (_sendq.size() > 9000)
			sleepms(1);
#ifdef DEBUG
#else
		if (count % 100000 == 0)
#endif
			printf("thread_publisher: Broadcast %7d sendq=%5ld\n", count, _sendq.size());
	}

	tmend = xgetclock();
	printf("thread_publisher: Send time=%.3f sec / %.3f TPS\n", tmend - tmstart, count / (tmend - tmstart));

	xpub.close();
	
	printf("thread_publisher: ----- END! count=%d\n\n", count);

	sleep(5);

	pthread_exit(NULL);

	return NULL;
}


//
// Send test thread (TX generation)
//
void	*thread_send_test(void *info_p)
{
        int	sendport = *(int *)info_p;
	int	loop = MAX_TEST_NUM_TX;			// 100


	printf("thread_send_test: loop=%d START!\n", loop);

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

	for (int ii = 0; ii < loop; ii++)
	{
		txdata_t txdata;
		tx_verify_reply_t txreply;

		txdata.valid = verify_check;

		txreply.type = TX_VERIFY_REPLY;
		txreply.status = (ii % 2 == 0) ? STAT_VERIFY_OK : STAT_VERIFY_FAIL;
		txreply.signature = sign.signature;
		txreply.txid = sha256(txreply.signature);

		printf("	SEND %s\n", get_status_name(txreply.status));

		xserial reply_txsz(1 * 1024);
		seriz_add(reply_txsz, txreply);

		txdata.data = reply_txsz.getstring();
		txdata.status = txdata.valid ? STAT_VERIFY_OK : STAT_VERIFY_FAIL;

		_sendq.push(txdata);

		if (_sendq.size() > 9000)
			sleepms(1);
#ifdef DEBUG
#else
		if (ii % 100000 == 0)
#endif
			printf("thread_send_test: Send %7d sendq=%5ld recvq=%5ld veriq=%5ld\n",
				ii, _sendq.size(), _verifyq.size(), _mempoolq.size());
	}

	printf("thread_send_test: ----- END!\n\n");

	pthread_exit(NULL);

	return NULL;
}


