#include "txcommon.h"


int	tx_verify(txdata_t& txdata);


//
// thread for VECTOR model
//
void	*thread_subscriber(void *info_p)
{
	char	*peer = (char *)info_p;
	int	count = 0;
	double	tmstart = 0, tmend = 0;

	FILE	*outfp = NULL;
	char	peerstr[100] = {0};
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

	tmstart = xgetclock();

	while (1)
	{
		txdata_t txdata;

		//  Wait for next request from publisher
		txdata.orgdataser = s_recv(xsock);

		printf("\n\n=====Subscriber for broadcast: (recv from ZMQ::PUB-SUB)\n");

		printf("    data length=%ld (include filter [%s])\n", txdata.orgdataser.size(), ZMQ_FILTER);

		const char *filter = txdata.orgdataser.c_str();

		string realdata(&filter[strlen(ZMQ_FILTER)], txdata.orgdataser.length() - strlen(ZMQ_FILTER));
		txdata.orgdataser = realdata;

		printf("    real data length=%ld\n", txdata.orgdataser.size());

		count++;
		tx_verify(txdata);

		if (txdata.hdr.status == STAT_VERIFY_REQUEST)
		{
			txdata.hdr.valid = -1;
			printf("    Add to verifyq: type=%s status=%s\n",
				get_type_name(txdata.hdr.type), get_status_name(txdata.hdr.status));

			_verifyq.push(txdata);			// send to verify.cpp
		}
		else if (txdata.hdr.status == STAT_VERIFY_OK)	// tx_verify_reply_t
		{
		//	tx_verify(txdata);
		//	_resultq.push(txdata);
		}
		else if (txdata.hdr.status == STAT_VERIFY_FAIL)	// tx_verify_reply_t
		{
		//	tx_verify(txdata);
		//	_resultq.push(txdata);
		}

		fprintf(outfp, "%7d: len=%ld\n", count, txdata.hdr.data_length);
		fflush(outfp);

#ifdef DEBUG
#else
		if (count % 100000 == 0)
#endif
			printf("    Processed %d  recvq=%5ld\n", count, _verifyq.size());
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
// Client thread (Client request management)
//
// 1. Verify tx and returns the result immediately.
// 2. Push into verifyq if valid tx. (status=STAT_INIT)
//
void	*thread_client(void *info_p)
{
        int	clientport = *(int *)info_p;
	char	tmp[256] = {0}, ip_port[100] = {0};


	printf("CLIENT: client port=%d START!\n", clientport);

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
		txdata.orgdataser = s_recv(responder);
		count++;

		printf("\n\n=====Subscriber for client: (recv from ZMQ::REQ-REPLY)\n");

		int verify_required = tx_verify(txdata);

		// 정상적으로 검증된 tx인 경우에만 verify 수행함 
		if (txdata.hdr.valid == 1 && verify_required)
		{
			txdata.hdr.status = STAT_INIT;
			printf("    Add to verifyq: type=%s status=%s\n",
				get_type_name(txdata.hdr.type), get_status_name(txdata.hdr.status));

			_verifyq.push(txdata);	// send to verify.cpp
		}

#ifdef DEBUG
#else
		if (count % 10000 == 0)
#endif
			printf("    Client request: count=%d sign=%s\n", count, txdata.hdr.signature.c_str());

		// Send reply back to client
		s_send(responder, txdata.hdr.txid);
	}

	fclose(outfp);

	printf("CLIENT: ----- END!\n\n");

	pthread_exit(NULL);

	return NULL;
}


//
// returns 1 if verify required
//
int	tx_verify(txdata_t& txdata)
{
	tx_header_t *hp;
	tx_create_token_t create_token;
	tx_send_token_t send_token;
	tx_verify_reply_t verify_reply;
	xserial hdrszr, bodyszr;
	string	from_addr;

	hdrszr.setstring(txdata.orgdataser);
	deseriz(hdrszr, txdata.hdr, 1);

	string body = hdrszr.getcurstring();
	bodyszr.setstring(body);

	txdata.bodyser = body;
	hp = &txdata.hdr;

	xserial tmpszr;
	seriz_add(tmpszr, txdata.hdr);
	txdata.hdrser = tmpszr.getstring();	// 헤더 serialization 교체

	if (hp->type == TX_CREATE_TOKEN)
	{
		deseriz(bodyszr, create_token, 0);
		from_addr = send_token.from_addr;

		hp->valid = verify_message_bin(from_addr.c_str(), hp->signature.c_str(), 
					txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
		printf("    verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());

		return 1;
	}
	else if (hp->type == TX_SEND_TOKEN)
	{
		deseriz(bodyszr, send_token, 0);
		from_addr = send_token.from_addr;

		hp->valid = verify_message_bin(from_addr.c_str(), hp->signature.c_str(), 
					txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
		printf("    verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());

		return 1;
	}
	else if (hp->type == TX_VERIFY_REPLY)
	{
		deseriz(bodyszr, verify_reply, 1);

		return 0;
	}
	else
	{
		printf("ERROR: Unknown TX type=%08X\n", hp->type);

		return 0;
	}
}
