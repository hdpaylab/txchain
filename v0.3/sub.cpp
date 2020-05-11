#include "txcommon.h"


//
// thread for VECTOR model
//
void	*thread_subscriber(void *info_p)
{
	char	*peer = (char *)info_p;
	int	count = 0;
	double	tmstart = 0, tmend = 0;

	FILE	*outfp = NULL;
	char	peerstr[100] = {0}, endmark[100] = {0};
	char	tmp[4096] = {0}, *tp = NULL;
	const char *filter = ZMQ_FILTER;


	printf("SUB : peer=%s START!\n", peer);

	zmq::context_t context_sub(1);
	zmq::socket_t xsock(context_sub, ZMQ_SUB);

	strcpy(tmp, peer);
	strcat(tmp, ".out");
	tp = strchr(tmp, ':');
	assert(tp != NULL);
	*tp = '_';
	outfp = fopen(tmp, "w+b");
	assert(outfp != NULL);

	// ZMQ setup 
	sprintf(peerstr, "tcp://%s", peer);
	xsock.connect(peerstr);
	
	xsock.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

	int bufsize = 1 * 1024 * 1024;	// 1MB buffer
	xsock.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	int qsize = 10000;		// 10000 
	xsock.setsockopt(ZMQ_RCVHWM, &qsize, sizeof(qsize));

	sleepms(100);

	sprintf(endmark, "%s CLOSE", filter);

	tmstart = xgetclock();

	while (1)
	{
		txdata_t txdata;

		txdata.data = s_recv(xsock);
		if (txdata.data == endmark)
		{
			fprintf(stderr, "Subscriber RECV CLOSE! peer=%s\n", peer);
			break;
		}

		count++;
		fprintf(outfp, "%7d: %s\n", count, txdata.data.c_str());
		fflush(outfp);
#ifdef DEBUG
#else
		if (count % 100000 == 0)
#endif
			printf("SUB : Recv %7d recvq=%5ld\n", count, _recvq.size());

		txdata.seq = count;
		txdata.verified = -1;
		txdata.status = TX_STATUS_SEND;

		_recvq.push(txdata);
	}

	fclose(outfp);

	tmend = xgetclock();
	printf("SUB : Recv time=%.3f / %.1f/sec\n",
		tmend - tmstart, count / (tmend - tmstart));

	fprintf(stderr, "\nSubscriber END! peer=%s count=%d\n", peer, count);

	xsock.close();

	sleep(5);

	pthread_exit(NULL);

	return NULL;
}


//
// Send client thread (Client management)
//
void	*thread_client(void *info_p)
{
        int	clientport = *(int *)info_p;
	char	tmp[256] = {0}, ip_port[100] = {0};


	printf("CLIENT: client port=%d START!\n", clientport);

	// params set
	Params_type_t params = paramsget("../lib/params.dat");

	snprintf(tmp, sizeof(tmp), "CLIENT %d.out", clientport);
	FILE *outfp = fopen(tmp, "w+b");	// output file
	assert(outfp != NULL);

	zmq::context_t context(1);

	zmq::socket_t responder(context, ZMQ_REP);
	sprintf(ip_port, "tcp://*:%d", clientport);
	responder.bind(ip_port);

	int bufsize = 64 * 1024;	// 64k buffer
	responder.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

	bufsize = 64 * 1024;		// 64k buffer
	responder.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	int	count = 0;

	while(1)
	{
		txdata_t txdata;
		tx_send_token_t txsend;
		xserial txsz(4 * 1024);
		char	retbuf[256] = {0};

		//  Wait for next request from client
		txdata.data = s_recv(responder);

		count++;

		txsz.setstring(txdata.data);

		txsz >> txsend.type;
		txsz >> txsend.status;
		txsz.rewind();

		printf("\n");
		printf("SUB:\n");
		deseriz(txsz, txsend, 1);
		deseriz(txsz, txdata.sign, 1);

		txdata.verified = verify_message_bin(txsend.from_addr.c_str(), txdata.sign.signature.c_str(), 
					txdata.data.c_str(), txdata.sign.data_length, &params.AddrHelper);

		printf("	VERITY	: %d\n", txdata.verified);
sleep(1);
		CSHA256 sha;
		unsigned char hash[CSHA256::OUTPUT_SIZE];	// 32B
		sha.Write(reinterpret_cast<const unsigned char*>(txdata.sign.signature.c_str()), txdata.sign.signature.length()).Finalize(hash);
		txdata.txid = bin2hex((const char *)hash, sizeof(hash));
	//	printf("HASH=%s\nDUMP:", txdata.txid.c_str()); 
	//	dumpbin((char *)hash, sizeof(hash), 0);

		txdata.valid = -1;
		txdata.status = TX_STATUS_READY;

		_recvq.push(txdata);	// send to verify.cpp

#ifdef DEBUG
#else
		if (count % 10000 == 0)
#endif
			cout << "Client request: count=" << count << " sign=" << txdata.sign.signature << endl;

		if (txdata.verified)
			snprintf(retbuf, sizeof(retbuf) - 1, "%s", txdata.txid.c_str());
		else
			snprintf(retbuf, sizeof(retbuf) - 1, "Transaction verification failed!\n");

		// Send reply back to client
		s_send(responder, retbuf);
	}

	fclose(outfp);

	printf("CLIENT: ----- END!\n\n");

	pthread_exit(NULL);

	return NULL;
}


