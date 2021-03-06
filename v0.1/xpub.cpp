#include "txcommon.h"


//
//
//
void	*thread_publisher(void *info_p)
{
        int	sendport = *(int *)info_p;
	int	count = 0;
	const char *filter = ZMQ_FILTER;
	const char ESC = TX_DELIM;
	char	msg[1024] = {0};
	double	tmstart = 0, tmend = 0;

	txmsg_t	txmsg;
	const char *message = "%d Hdac Technology, 잘 가는지 검사하는 것임23456789 ";
	const char *pubkey = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";

	sprintf(msg, message, sendport);
	message = msg;
	txmsg.message = strdup(message);
	txmsg.pubkey = strdup(pubkey);

	// params set
	Params_type_t params = load_params("../lib/params.dat");

	for (int loop = 1; loop <= 1; loop++)
	{
		char	bindstr[100] = {0}, data[4096] = {0}, tmp[1024] = {0};

		fprintf(stderr, "Publisher: %d START! sendport=%d\n\n", loop, sendport);

		//  Prepare our context and publisher
		zmq::context_t context_pub(1);
		zmq::socket_t xpub(context_pub, ZMQ_PUB);
		sprintf(bindstr, "tcp://*:%d", sendport);
		xpub.bind(bindstr);

		int bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
		xpub.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

		// Avoiding message loss
		int one = 1;
		xpub.setsockopt(ZMQ_XPUB_NODROP, &one, sizeof(one));

		// max send message length
		int qsize = 10000;		// 10000 개
		xpub.setsockopt(ZMQ_SNDHWM, &qsize, sizeof(qsize));

		sleep(2);

		fprintf(stderr, "Publisher: %d START SEND!\n\n", loop);
		fprintf(stderr, "Message=%s\n\n", txmsg.message);

		txmsg.signature = sign_message(
			"LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm",	// privkey
			txmsg.message,
			&params.PrivHelper, &params.AddrHelper);

		/*****
		printf("xp:pubkey	: [%s]\n", txmsg.pubkey);
		printf("xp:message	: [%s]\n", txmsg.message);
		printf("xp:signature	: [%s]\n", txmsg.signature);

		int verify_check = verify_message(txmsg.pubkey, txmsg.signature, txmsg.message, &params.AddrHelper);
		printf("xp:verify_check=%d\n", verify_check);
		*****/

		printf("Signature: %s\n\n", txmsg.signature);
		tmstart = xgetclock();

		for (int ii = 0; ii < 1000000; ii++)
		{
			// send 260 bytes
			count++;
			memset(tmp, 0x00, sizeof(tmp));

			sprintf(data, "%s%7d %c%s%c%s%c%s", 
				filter, count, ESC, txmsg.pubkey, ESC, txmsg.message, ESC, txmsg.signature);
			bool ret = s_send(xpub, data);

			printf("SEND %d msg=%s\n", count, msg);
			sleep(1);

			// send 260 bytes
		//	count++;
		//	if (count % 100000 == 0)
		//		printf("PUB: Send %d\n", count);

		//	ret = s_send(xpub, data);

		//	cout << "s_send: " << data << endl;

		//	처음 문자열이 filter가 아니면 수신하지 못함
		//	sprintf(data, "1234 %s pubid=%d: s_sendmore DATA %d", filter, loop, rand());
		//	s_sendmore(xpub, data);
		//	cout << "s_sendmore: " << data << endl;

		//	s_sendmore(xpub, "TEST");
		//	cout << "s_sendmore: " << "TEST" << endl;
		//	s_send(xpub, data);
		//	cout << "s_send: " << data << endl;

		}

		tmend = xgetclock();
		printf("PUB: Send time=%.3f sec\n", tmend - tmstart);

		free(txmsg.signature);
		txmsg.signature = NULL;

		sprintf(data, "%s CLOSE", filter);
		printf("\n\nSEND: %s\n", data);
		s_send(xpub, data);

		fprintf(stderr, "Publisher: FINISH! sendport=%d\n", sendport);

		sleep(1);
		fprintf(stderr, "Publisher: END! sendport=%d\n", sendport);

		xpub.close();
	}
	
	sleep(5);

	pthread_exit(NULL);

	return 0;
}

