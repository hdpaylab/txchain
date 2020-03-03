#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "data_control.h"

using namespace std;

void *publish(void *info_p)
{
        char *info;
        info = (char*)info_p;

        PGconn *conn = PQconnectdb("hostaddr=127.0.0.1 \
                        user=postgres \
                        password=postgres \
                        dbname=testdb");

        if (PQstatus(conn) == CONNECTION_BAD) {
                printf("db connect failed\n");
        }

	//  Prepare our context and publisher
	zmq::context_t context_pub (1);

	zmq::socket_t publisher (context_pub, ZMQ_PUB);
	publisher.bind("tcp://*:5556");

	// data select
	// string tx = tx_get(conn);
	string tx = "test text";

	//  Send message to all subscribers
	zmq::message_t message(200);
	snprintf ((char *) message.data(), tx.size(), tx.data());
	publisher.send(message);

	cout << tx << endl;
	
        PQfinish(conn);

	return 0;
}

