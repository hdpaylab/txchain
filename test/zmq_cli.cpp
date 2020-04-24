#include <string>
#include "zhelpers.hpp"


using namespace std;


void	req_rep();
void	push_pull(int direc);


int	main(int argc, char *argv[])
{
//	req_rep();

	push_pull(1);	// 1=signle direction 2=both direction
}


//
// Request - Reply model
//
void	req_rep()
{
	zmq::context_t context(1);

	zmq::socket_t requester(context, ZMQ_REQ);
	requester.connect("tcp://192.168.1.10:7070");
	requester.connect("tcp://192.168.1.10:7071");	// 여러 서버로 보낼 때는, N번 보내야 각 서버로 전송됨 
	requester.connect("tcp://192.168.1.10:7072");
	int nsvr = 3;

	int bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
	requester.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
	requester.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

	const char *data = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=";

	for (int count = 0; count < 100; count++)
	{
		char	msg[100] = {0};

		snprintf(msg, sizeof(msg) - 1, "SEND %5d %s", count, data);

		for (int ii = 0; ii < nsvr; ii++)
		{
			s_send(requester, msg);

			string reply = s_recv(requester);
		//	if (count % 10000 == 0)
				cout << "Received reply " << count << " [" << reply << "]" << endl;
		}

		sleep(1);
	}
}


//
// Request - Reply model
//
void	push_pull(int direc)
{
	zmq::context_t context(1);

	//  Socket to receive messages on
	zmq::socket_t receiver(context, ZMQ_PULL);
	if (direc == 2)
	{
		receiver.bind("tcp://*:5557");
	}

	//  Socket to send messages to
	zmq::socket_t sender(context, ZMQ_PUSH);
	sender.connect("tcp://localhost:5558");

	uint64_t count = 0;
	uint64_t sendsz = 0, recvsz = 0;
	const char *data = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=";

	//  Process tasks forever
	while (1)
	{
		zmq::message_t message;
		int workload;           //  Workload in msecs

		//  Send results to sink
		count++;
		s_send(sender, data);

		sendsz += strlen(data);
		if (count % 10000 == 0)
			printf("SEND: %lu %.3g Mbytes\n", count, sendsz / 1000000.0);

		if (direc == 2)
		{
			receiver.recv(&message);
			std::string smessage(static_cast<char*>(message.data()), message.size());
			recvsz += smessage.length();
			if (count % 10000 == 0)
				printf("RECV: %lu %.3g Mbytes\n", count, recvsz / 1000000.0);
		}
/***
		std::istringstream iss(smessage);
		iss >> workload;

		//  Do the work
		s_sleep(workload);
***/

	}
}
