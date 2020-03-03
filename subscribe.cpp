#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>

#include "data_control.h"

using namespace std;

void *subscribe(void *info_p)
{
	char *info;
	info = (char*)info_p;

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

		zmq::context_t context_sub (1);
		zmq::socket_t subscriber (context_sub, ZMQ_SUB);

		subscriber.connect("tcp://192.168.1.10:5556");
		
		const char *filter = "tx";
		subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));

		while (1)
		{
			sleep(1);

			printf("---1\n");
			zmq::message_t update;
			subscriber.recv(&update);

			string recv_msg = string(static_cast<char*>(update.data()), update.size());

			cout << "RECV: " << recv_msg << endl;

			// tx_processing(recv_msg);

		}
	}

	/***
	tx_save(conn, recv_msg);

        PQfinish(conn);
	***/

	return 0;
}

