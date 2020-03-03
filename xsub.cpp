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
		sleep (1);

		printf("Subscriber started!\n\n");

		zmq::context_t context_sub(1);
		zmq::socket_t xsock(context_sub, ZMQ_SUB);

		xsock.connect("tcp://localhost:5556");
		
		const char *filter = "!@#$";	// s_sendmore()로 publish에서 보내는 것만 수용함 
		xsock.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

		while (1)
		{
			std::string data = s_recv(xsock);

			printf("RECV(cliid=%d): %s\n", count, data.c_str());
			count++;
		}

		printf("\r%d    \n", count);
	}

	/***
	tx_save(conn, recv_msg);

        PQfinish(conn);
	***/

	return 0;
}

