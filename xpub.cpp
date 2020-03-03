#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <unistd.h>

#include "zhelpers.hpp"

#include "xdb.h"

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

	for (int loop = 1; loop < 5; loop++)
	{
		//  Prepare our context and publisher
		zmq::context_t context_pub(1);
		zmq::socket_t xpub(context_pub, ZMQ_PUB);
		xpub.bind("tcp://*:5556");

		printf("Publisher: %d START!\n\n", loop);

	//	while (1)
		for (int ii = 0; ii < 10; ii++)
		{
			usleep(500 * 1000);

			char	data[256] = {0};
			char	*filter = "!@#$";

			sprintf(data, "%s pubid=%d: s_sendmore DATA %d", filter, loop, rand());

			s_sendmore(xpub, data);
		//	cout << "s_sendmore: " << data << endl;

			sprintf(data, "%s pubid=%d: s_send DATA %d", filter, loop, rand());

			s_send(xpub, data);
		//	cout << "s_send: " << data << endl;

			sprintf(data, "1234 %s pubid=%d: s_sendmore DATA %d", filter, loop, rand());

			s_sendmore(xpub, data);
		//	cout << "s_sendmore: " << data << endl;

			s_sendmore(xpub, "TEST");
		//	cout << "s_sendmore: " << "TEST" << endl;
			s_send(xpub, data);
		//	cout << "s_send: " << data << endl;
		}

		xpub.close();
		printf("Publisher: END!\n");
	}
	
	/***
        PQfinish(conn);
	***/

	return 0;
}

