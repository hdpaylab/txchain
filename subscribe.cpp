#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>

#include "zhelpers.hpp"
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

		zmq::context_t context_sub(1);
		zmq::socket_t xsub(context_sub, ZMQ_SUB);

		xsub.connect("tcp://localhost:5556");
		
		const char *filter = "TX";	// s_sendmore()로 publish에서 보내는 것만 수용함 
		xsub.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

		while (1)
		{
			sleep(1);

			/***
			zmq::message_t update;
			xsub.recv(&update);

			printf("---2\n");
			string recv_msg = string(static_cast<char*>(update.data()), update.size());

			cout << "RECV: " << recv_msg << endl;

			// tx_processing(recv_msg);
			***/

			std::string data = s_recv(xsub);

			cout << "RECV: " << data << endl;

		}
	}

	/***
	tx_save(conn, recv_msg);

        PQfinish(conn);
	***/

	return 0;
}

