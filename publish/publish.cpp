#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "data_get.h"

using namespace std;

int publish(PGconn *conn)
{
	//  Prepare our context and publisher
	zmq::context_t context_pub (1);

	zmq::socket_t publisher (context_pub, ZMQ_PUB);
	publisher.bind("tcp://*:5556");

	// data select
	string tx = tx_get(conn);

	//  Send message to all subscribers
	zmq::message_t message(200);
	snprintf ((char *) message.data(), tx.size(), tx.data());
	publisher.send(message);

	cout << tx << endl;

	return 0;
}

