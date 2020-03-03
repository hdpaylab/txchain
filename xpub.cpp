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
        char	*info = (char*)info_p;
	int	count = 0;

	/****
        PGconn *conn = PQconnectdb("hostaddr=127.0.0.1 \
                        user=postgres \
                        password=postgres \
                        dbname=testdb");

        if (PQstatus(conn) == CONNECTION_BAD) {
                printf("db connect failed\n");
        }
	***/

	for (int loop = 1; loop <= 1; loop++)
	{
		fprintf(stderr, "Publisher: %d START!\n\n", loop);

		//  Prepare our context and publisher
		zmq::context_t context_pub(1);
		zmq::socket_t xpub(context_pub, ZMQ_PUB);
		xpub.bind("tcp://*:5556");

		char	data[256] = {0}, tmp[200] = {0};
		char	*filter = "!@#$";

		sleep(2);

		fprintf(stderr, "Publisher: %d START SEND!\n\n", loop);

		for (int ii = 0; ii < 100000; ii++)
		{
			usleep(1);

			// send 100 bytes
			count++;
			strcpy(tmp, "1234567890");
			sprintf(data, "%s%6d----------%s%s%s%s%s%s%s%s", filter, count,
				tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp);
			s_sendmore(xpub, data);
		//	cout << "s_sendmore: " << data << endl;

			// send 100 bytes
			count++;
			strcpy(tmp, "1234567890");
			sprintf(data, "%s%6d==========%s%s%s%s%s%s%s%s", filter, count,
				tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp);

			s_send(xpub, data);
		//	cout << "s_send: " << data << endl;

		//	처음 문자열이 filter가 아니면 수신하지 못함
		//	sprintf(data, "1234 %s pubid=%d: s_sendmore DATA %d", filter, loop, rand());
		//	s_sendmore(xpub, data);
		//	cout << "s_sendmore: " << data << endl;

		//	s_sendmore(xpub, "TEST");
		//	cout << "s_sendmore: " << "TEST" << endl;
		//	s_send(xpub, data);
		//	cout << "s_send: " << data << endl;
		}

		sprintf(data, "%s CLOSE", filter);
		printf("SEND: %s\n", data);
		s_send(xpub, data);

		fprintf(stderr, "Publisher: FINISH!\n");

		sleep(1);
		fprintf(stderr, "Publisher: END!\n");

		xpub.close();
	}
	
	/***
        PQfinish(conn);
	***/

	pthread_exit(NULL);

	return 0;
}

