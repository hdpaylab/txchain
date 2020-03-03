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

using namespace std;


void *subscribe(void *info_p)
{
	char	*info = (char*)info_p;
	int	count = 0;

	/***
	PGconn *conn = PQconnectdb("hostaddr=127.0.0.1 \
			user=postgres \
			password=postgres \
			dbname=testdb");

	if (PQstatus(conn) == CONNECTION_BAD) {
		printf("db connect failed\n");
	}
	***/

	while (1)
	{
		fprintf(stderr, "Subscriber START!\n\n");

		zmq::context_t context_sub(1);
		zmq::socket_t xsock(context_sub, ZMQ_SUB);

		xsock.connect("tcp://192.168.1.10:5556");
		
		char	endmark[100] = {0};
		const char *filter = "!@#$";	// s_sendmore()로 publish에서 보내는 것만 수용함 

		xsock.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

		sprintf(endmark, "%s CLOSE", filter);

		sleep (1);

		fprintf(stderr, "Subscriber START RECV!\n\n");

		while (1)
		{
			std::string data = s_recv(xsock);

			count++;
			printf("RECV(cliid=%d): %s\n", count, data.c_str());
			if (data == endmark)
			{
				fprintf(stderr, "Subscriber RECV CLOSE!\n");
				break;
			}
			else if (count % 100 == 0)
			{
				fprintf(stderr, "\r%8d    ", count);
				fflush(stderr);
			}
		}

		fprintf(stderr, "\r%8d    \n", count);

		fprintf(stderr, "\nSubscriber END!\n");

		xsock.close();

		break;
	}

	/***
	tx_save(conn, recv_msg);

        PQfinish(conn);
	***/

	pthread_exit(NULL);

	return 0;
}

