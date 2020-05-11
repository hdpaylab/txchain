#include "txcommon.h"


//
// Send test thread (TX generation)
//
void	*thread_send_test(void *info_p)
{
	int	loop = MAX_TEST_NUM_TX;			// 100

	const char *filter = ZMQ_FILTER;
	const char ESC = TX_DELIM;
	const char *message = "Hdac Technology, 잘 가는지 테스트 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789";
	const char *address = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";
	char	data[1024];
	double	tmstart = 0, tmend = 0;


	printf("SEND: loop=%d START!\n", loop);

	// params set
	Params_type_t params = paramsget("../lib/params.dat");

	txmsg_t	txmsg;
	txmsg.message = strdup(message);
	txmsg.address = strdup(address);
	txmsg.signature = sign_message(
		"LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm",	// privkey
		txmsg.message,
		&params.PrivHelper, &params.AddrHelper);

	/*****
	printf("xp:address	: [%s]\n", txmsg.address);
	printf("xp:message	: [%s]\n", txmsg.message);
	printf("xp:signature	: [%s]\n", txmsg.signature);

	int verify_check = verify_message(txmsg.address, txmsg.signature, txmsg.message, &params.AddrHelper);
	printf("xp:verify_check=%d\n", verify_check);
	*****/

	sleepms(500);

	tmstart = xgetclock();

	for (int ii = 0; ii < loop; ii++)
	{
		txdata_t txdata;

		snprintf(data, sizeof(data), "%s%7d %c%s%c%s%c%s", 
			filter, ii, ESC, txmsg.address, ESC, txmsg.message, ESC, txmsg.signature);

		txdata.data = data;
		txdata.seq = ii + 1;
		txdata.valid = -1;
		txdata.verified = -1;
		txdata.status = TX_STATUS_SEND;

		_sendq.push(txdata);

#ifdef DEBUG
		sleepms(DEBUG_SLEEP_MS);
	//	if (ii % 10 == 0)
#else
		if (ii % 100000 == 0)
#endif
			printf("SEND: Send %7d sendq=%5ld recvq=%5ld veriq=%5ld\n",
				ii, _sendq.size(), _recvq.size(), _veriq.size());
	}

	snprintf(data, sizeof(data), "%s CLOSE", filter);
	printf("\n\nSEND: %s\n", data);

	txdata_t txdata;
	txdata.data = data;
	txdata.seq = MAX_SEQ;
	txdata.verified = -1;
	txdata.status = TX_STATUS_REPLY;

	_sendq.push(txdata);

	tmend = xgetclock();
	printf("SEND: Send time=%.3f sec / %.3f TPS\n", tmend - tmstart, loop / (tmend - tmstart));

	printf("SEND: ----- END!\n\n");

	pthread_exit(NULL);

	return NULL;
}


//
// Publisher - Sending TX to other nodes
//
void	*thread_publisher(void *info_p)
{
        int	sendport = *(int *)info_p;
	int	count = 0;
	double	tmstart = 0, tmend = 0;
	char	bindstr[100] = {0};


	printf("PUB : sendport=%d START!\n", sendport);

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

		bool ret = s_send(xpub, txdata.data);

		if (txdata.seq == MAX_SEQ) break;

#ifdef DEBUG
	//	sleepms(DEBUG_SLEEP_MS);
#else
		if (count % 100000 == 0)
#endif
			printf("PUB : Send %7d sendq=%5ld\n", count, _sendq.size());
	}

	tmend = xgetclock();
	printf("PUB : Send time=%.3f sec / %.3f TPS\n", tmend - tmstart, count / (tmend - tmstart));

	xpub.close();
	
	printf("PUB : ----- END! count=%d\n\n", count);

	sleep(5);

	pthread_exit(NULL);

	return NULL;
}

