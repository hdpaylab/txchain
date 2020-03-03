#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <unistd.h>

#include "zhelpers.hpp"

#include "data_control.h"

using namespace std;

void *publish(void *info_p)
{
        char *info;
        info = (char*)info_p;

	/****
        PGconn *conn = PQconnectdb("hostaddr=127.0.0.1 \
                        user=postgres \
                        password=postgres \
                        dbname=testdb");

        if (PQstatus(conn) == CONNECTION_BAD) {
                printf("db connect failed\n");
        }
	***/

	//  Prepare our context and publisher
	zmq::context_t context_pub(1);
	zmq::socket_t xpub(context_pub, ZMQ_PUB);
	xpub.bind("tcp://*:5556");

	printf("Publisher started!\n\n");

	while (1)
	{
		sleep(1);

		char	data[256] = {0};

		sprintf(data, "DATA %d", rand());

		s_sendmore(xpub, "TX");
		s_send(xpub, data);

		s_sendmore(xpub, "TEST");
		s_send(xpub, data);

		/*****
		// data select
		// string tx = tx_get(conn);
		string tx = "test text";

		//  Send message to all subscribers
		zmq::message_t message(200);
		snprintf ((char *) message.data(), tx.size(), tx.data());
		xpub.send(message);
		***/

		cout << "SEND: " << data << endl;
	}
	
	/***
        PQfinish(conn);
	***/

	return 0;
}

