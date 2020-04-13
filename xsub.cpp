#include "txcommon.h"


//
// thread for VECTOR model
//
void	*thread_subscriber(void *info_p)
{
	char	*peer = (char *)info_p;
	int	count = 0;
	pthread_t thrid[4];
	int	index = 0;
	int	ret = 0;
	double	tmstart = 0, tmend = 0;


	// Verifier thread creation
	for (index = 0; index < _nthread; index++)
	{
		int	id = index;
	        ret = pthread_create(&thrid[index], NULL, thread_verifier, (void *)&id);
                if (ret < 0) {
                        perror("thread create error : ");
                        return 0;
                }
                usleep(10 * 1000);

	}
	for (index = 0; index < _nthread; index++)
	{
		pthread_detach(thrid[index]);
	}

	printf("XSUB: Verifier threads created.\n");

	for (int ii = 0; ii < MAX_VECTOR_SIZE; ii++)
	{
		_txv[ii].verified = -1;
		_txv[ii].status = TXCHAIN_STATUS_EMPTY;
	}

	while (1)
	{
		FILE	*outfp = NULL;
		char	peerstr[100] = {0}, endmark[100] = {0};
		char	tmp[4096] = {0}, *tp = NULL;
		const char *filter = "!@#$";	// s_sendmore()로 publisher에서 보내는 것만 수용함 

		fprintf(stderr, "Subscriber START! peer=%s\n\n", peer);

		zmq::context_t context_sub(1);
		zmq::socket_t xsock(context_sub, ZMQ_SUB);

		strcpy(tmp, peer);
		strcat(tmp, ".out");
		tp = strchr(tmp, ':');
		assert(tp != NULL);
		*tp = '_';
		outfp = fopen(tmp, "w+b");	// 출력파일 *.out
		assert(outfp != NULL);

		sprintf(peerstr, "tcp://%s", peer);
		xsock.connect(peerstr);
		
#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

		// message queue connect
 		int msqid = msgget( (key_t)SUBSCRIBER_MSGQ_ID, IPC_CREAT | 0666);
		if (msqid == -1) {
			perror("msgget(SUBSCRIBER_MSGQ_ID)");
			exit(-1);
		}

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ

		xsock.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

		int bufsize = 4 * 1024 * 1024;	// 4MB 버퍼 
		xsock.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

		int qsize = 100000;	// 10000 개
		xsock.setsockopt(ZMQ_RCVHWM, &qsize, sizeof(qsize));

		sprintf(endmark, "%s CLOSE", filter);

		sleep (1);

		fprintf(stderr, "Subscriber START RECV! peer=%s\n\n", peer);
		tmstart = xgetclock();

		while (1)
		{
			std::string data = s_recv(xsock);

			if (data == endmark)
			{
				fprintf(stderr, "Subscriber RECV CLOSE! peer=%s\n", peer);
				break;
			}

			count++;
			fprintf(outfp, "%d: %s\n", count, data.c_str());
			if (count % 100000 == 0)
				printf("SUB: Receive %d\n", count);

#ifdef TXCHAIN_VERIFY_MODEL_VECTOR

			// 아직 처리가 안되었으면 대기..
			int idx = _push_count % MAX_VECTOR_SIZE;
			while (_txv[idx].status != TXCHAIN_STATUS_EMPTY)
			{
#ifdef DEBUG_SLOW_MODE
				printf("XSUB: Wait EMPTY...\n");
				usleep(500 * 1000);
#else
				usleep(10);
#endif
			}

			txdata_t tx;

			tx.data = data;
			tx.verified = -1;
			tx.status = TXCHAIN_STATUS_READY;

			_txv[idx] = tx;
			_push_count++;
#ifdef DEBUG_SLOW_MODE
			usleep(100 * 1000);
#else
			if (_push_count % 100000 == 0)
#endif
				printf("Add[%d]: READY count=%d vsize=%ld\n", idx, _push_count, _txv.size());

#endif	// TXCHAIN_VERIFY_MODEL_VECTOR

#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

			data_t msq_data;

			msq_data.mtype = 1;
			strncpy(msq_data.mtext, data.c_str(), data.length());

			// message queue send
			if (msgsnd(msqid, 
				&msq_data, data.length(), 0) == -1) {
				fprintf(stderr, "ERROR: message queue send error - %d: %s\n",
					count, data.c_str());
			}

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ
		}

		tmend = xgetclock();
		printf("SUB: Receive time=%.3f / %.1f/sec\n",
			tmend - tmstart, count / (tmend - tmstart));

		fclose(outfp);

		fprintf(stderr, "\r%s	%8d    \n", peer, count);

		fprintf(stderr, "\nSubscriber END! peer=%s\n", peer);

		xsock.close();

		break;
	}

	sleep(5);

	pthread_exit(NULL);

	return 0;
}
