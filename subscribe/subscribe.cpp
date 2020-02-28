#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>

#include "data_save.h"

using namespace std;

int subscribe(PGconn *conn)
{
	zmq::context_t context_sub (1);
	zmq::socket_t subscriber (context_sub, ZMQ_SUB);

	subscriber.connect("tcp://192.168.70.140:5556");
	
	const char *filter = "100 ";
	subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));

	zmq::message_t update;
	subscriber.recv(&update);

	string recv_msg = 
		string(static_cast<char*>(update.data()), update.size());

	cout << recv_msg << endl;

	// tx_processing(recv_msg);

	tx_save(conn, recv_msg);

	return 0;
}

