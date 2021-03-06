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

		printf("\n-----Publisher for PUB-SUB:\n");
		printf("    recv data length=%ld filter=%s\n", txdata.orgdataser.size(), ZMQ_FILTER);

		string senddata;

		senddata = string(ZMQ_FILTER) + txdata.hdrser + txdata.bodyser;

//		printf("SEND DUMP(%ld): ", senddata.length()); 
//		dumpbin(senddata.c_str(), senddata.length());

		bool ret = s_send(xpub, senddata);

		if (_sendq.size() > 9000)
			sleepms(1);
#ifdef DEBUG
#else
		if (count % 100000 == 0)
#endif
			printf("    Send: broadcast %d  sendq=%5ld\n", count, _sendq.size());
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

	const char *privkey = "LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm";	// privkey
	const char *from_addr = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";	
	const char *to_addr = "HUGUrwcFy1VC91nq7tRuZpaJqndoHDw64e";

	tx_header_t txhdr;
	tx_send_token_t	txsend;

	memset(&txhdr, 0, sizeof(txhdr));
	txhdr.nodeid = getpid();		// 임시로 
	txhdr.type = TX_SEND_TOKEN;
	txhdr.valid = -1;
	
	txsend.from_addr = from_addr;
	txsend.to_addr = to_addr;
	txsend.token_name = "XTOKEN";
	txsend.amount = 123.456;
	txsend.native_amount = 0.0;
	txsend.fee = 0.0;
	txsend.user_data = "TEST SEND TOKEN by THREAD";
	txsend.sign_clock = xgetclock();

	xserialize hdrszr, bodyszr;

	seriz_add(bodyszr, txsend);
	txhdr.data_length = bodyszr.size();

	txhdr.data_length = bodyszr.size();
	txhdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), &_netparams.PrivHelper, &_netparams.AddrHelper);
	printf("Serialize: body length=%ld\n", bodyszr.size());
	printf("address  : %s\n", from_addr);
//	printf("message  : \n"); bodyszr.dump(10, 1);
	printf("signature: %s\n", txhdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(from_addr, txhdr.signature.c_str(), bodyszr.data(), bodyszr.size(), &_netparams.AddrHelper);
	printf("verify_check=%d\n", verify_check);
	printf("\n");

	// sign은 verify 테스트 전에 serialize하면 안됨..
	seriz_add(hdrszr, txhdr);

	for (int ii = 0; ii < loop; ii++)
	{
		txdata_t txdata;

		/*
		txhdr.nodeid = getpid();
		txhdr.type = TX_VERIFY_REPLY;
		txhdr.status = 0;
		txhdr.data_length = 0;
		txhdr.valid = verify_check;

		txhdr.txid = sha256(txhdr.signature);

		printf("	SEND REPLY valid=%d\n", txhdr.valid);

		xserialize hdrszr;
		seriz_add(hdrszr, txhdr);
		*/

		txdata.orgdataser = hdrszr.getstring() + bodyszr.getstring();

		printf("    Add to sendq: type=%s status=%s\n",
			get_type_name(txhdr.type), get_status_name(txhdr.status));

		_sendq.push(txdata);

		if (_sendq.size() > 9000)
			sleepms(1);
#ifdef DEBUG
		sleep(1);
#else
		if (ii % 100000 == 0)
#endif
			printf("    Auto sender: send %d  sendq=%5ld\n", ii, _sendq.size());
	}

	printf("thread_send_test: ----- END!\n\n");

	pthread_exit(NULL);

	return NULL;
}


