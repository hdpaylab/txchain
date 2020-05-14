#include "txcommon.h"


string	process_tx(txdata_t& txdata);


Params_type_t _params;


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

		//  Wait for next request from publisher
		txdata.data = s_recv(xsock);

		count++;
		process_tx(txdata);

		if (txdata.status == STAT_VERIFY_REQUEST)
		{
			txdata.valid = -1;
			_verifyq.push(txdata);			// send to verify.cpp
		}
		else if (txdata.status == STAT_VERIFY_OK)	// tx_verify_reply_t
		{
			_resultq.push(txdata);
		}
		else if (txdata.status == STAT_VERIFY_FAIL)	// tx_verify_reply_t
		{
			_resultq.push(txdata);
		}

		fprintf(outfp, "%7d: %s\n", count, txdata.data.c_str());
		fflush(outfp);

#ifdef DEBUG
#else
		if (count % 100000 == 0)
#endif
			printf("SUB : Recv %7d recvq=%5ld\n", count, _verifyq.size());

		txdata.seq = count;
		txdata.valid = -1;
		txdata.status = STAT_INIT;

		_verifyq.push(txdata);
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
	_params = paramsget("../lib/params.dat");

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

		//  Wait for next request from client
		txdata.data = s_recv(responder);

		count++;
		process_tx(txdata);

		txdata.valid = -1;
		txdata.status = STAT_INIT;

		_verifyq.push(txdata);	// send to verify.cpp

#ifdef DEBUG
#else
		if (count % 10000 == 0)
#endif
			cout << "Client request: count=" << count << " sign=" << txdata.sign.signature << endl;

		// Send reply back to client
		s_send(responder, txdata.txid);
	}

	fclose(outfp);

	printf("CLIENT: ----- END!\n\n");

	pthread_exit(NULL);

	return NULL;
}


string	process_tx(txdata_t& txdata)
{
	tx_create_token_t create_token;
	tx_send_token_t send_token;
	tx_verify_reply_t verify_reply;
	xserial txsz(4 * 1024);
	string	from_addr, retstr;
	uint32_t type, status;

	txsz.setstring(txdata.data);

	txsz >> type;
	txsz >> status;
	txsz.rewind();

	printf("\n");
	printf("-----SUB:\n");
	switch (type)
	{
	case TX_CREATE_TOKEN:
	//	deseriz(txsz, create_token, 1);
		deseriz(txsz, txdata.sign, 1);
		from_addr = send_token.from_addr;
		break;

	case TX_SEND_TOKEN:
		deseriz(txsz, send_token, 1);
		deseriz(txsz, txdata.sign, 1);
		from_addr = send_token.from_addr;
		break;

	case TX_VERIFY_REPLY:
		deseriz(txsz, verify_reply, 1);
		return string();

	default:
		printf("ERROR: Unknown TX type=%08X\n", type);
		return retstr;
	}

	txdata.valid = verify_message_bin(from_addr.c_str(), txdata.sign.signature.c_str(), 
				txdata.data.c_str(), txdata.sign.data_length, &_params.AddrHelper);
	printf("	VERITY	: %d\n", txdata.valid);

	if (txdata.valid)
	{
		txdata.txid = sha256(txdata.sign.signature);

		printf("HASH=%s\n", txdata.txid.c_str()); 
	}
	else
	{
		txdata.txid = "ERROR: Transaction verification failed!";
	}

	return txdata.txid;
}
