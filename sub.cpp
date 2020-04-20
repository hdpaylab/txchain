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
	const char *filter = ZMQ_FILTER;	// s_sendmore()로 publisher에서 보내는 것만 수용함 


	printf("SUB : peer=%s START!\n", peer);

	zmq::context_t context_sub(1);
	zmq::socket_t xsock(context_sub, ZMQ_SUB);

	strcpy(tmp, peer);
	strcat(tmp, ".out");
	tp = strchr(tmp, ':');
	assert(tp != NULL);
	*tp = '_';
	outfp = fopen(tmp, "w+b");	// 출력파일 *.out
	assert(outfp != NULL);

	// ZMQ setup 
	sprintf(peerstr, "tcp://%s", peer);
	xsock.connect(peerstr);
	
	xsock.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

	int bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
	xsock.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	int qsize = 10000;		// 10000 개
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
		sleepms(DEBUG_SLEEP);
	//	if (count % 10 == 0)
#else
		if (count % 100000 == 0)
#endif
			printf("SUB : Recv %7d recvq=%5ld\n", count, _recvq.size());

		txdata.seq = count;
		txdata.verified = TXCHAIN_STATUS_EMPTY;
		txdata.status = TXCHAIN_STATUS_EMPTY;

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
