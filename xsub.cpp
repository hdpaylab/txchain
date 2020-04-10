#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>

#include "zhelpers.hpp"
#include "xdb.h"
#include "xparams.h"
#include "xverify.h"
#include "xmsq.h"

using namespace std;


void	*thread_subscriber(void *info_p)
{
	char	*peer = (char *)info_p;
	int	count = 0;
	pthread_t thrid[4];
	int	index = 0;
	int	ret = 0;
	int	msqid = -1;

	for (index = 0; index <= MAX_VERIFY; index++)
	{
		int	id = index + 1;
	        ret = pthread_create(&thrid[index], NULL, thread_verify, (void *)&id);
                if (ret < 0) {
                        perror("thread create error : ");
                        return 0;
                }
                usleep(10 * 1000);

	}
	for (index = 0; index <= MAX_VERIFY; index++) {
		pthread_detach(thrid[index]);
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
		outfp = fopen(tmp, "w+b");
		assert(outfp != NULL);

		sprintf(peerstr, "tcp://%s", peer);
		xsock.connect(peerstr);
		
		// message queue connect
 		msqid = msgget( (key_t)1234, IPC_CREAT | 0666);
		if (msqid == -1) {
			perror("msgget(1234)");
		}

		xsock.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

		int bufsize = 4 * 1024 * 1024;	// 4MB 버퍼 
		xsock.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

		int qsize = 100000;	// 10000 개
		xsock.setsockopt(ZMQ_RCVHWM, &qsize, sizeof(qsize));

		sprintf(endmark, "%s CLOSE", filter);

		sleep (1);

		fprintf(stderr, "Subscriber START RECV! peer=%s\n\n", peer);

		while (1)
		{
			data_t msq_data;

			std::string data = s_recv(xsock);

			if (data == endmark)
			{
				fprintf(stderr, "Subscriber RECV CLOSE! peer=%s\n", peer);
				break;
			}

			count++;
			fprintf(outfp, "%d: %s\n", count, data.c_str());

			msq_data.mtype = 1;
			snprintf(msq_data.mtext, BUFF_SIZE, "%s", data.c_str());

			// message queue send
			if (msgsnd(msqid, 
				&msq_data, strlen(msq_data.mtext), 0) == -1) {
				fprintf(stderr, "ERROR: message queue send error - %d: %s\n",
					count, data.c_str());
			}
		}

		fclose(outfp);

		fprintf(stderr, "\r%s	%8d    \n", peer, count);

		fprintf(stderr, "\nSubscriber END! peer=%s\n", peer);

		xsock.close();

		break;
	}

	/***
        PQfinish(conn);
	***/

	sleep(1);

	pthread_exit(NULL);

	return 0;
}

