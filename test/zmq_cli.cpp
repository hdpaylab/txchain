#include <string>
#include "zhelpers.hpp"


using namespace std;


void	req_rep(int ac, char *av[]);
void	push_pull(int direc);


int	main(int ac, char *av[])
{
	req_rep(ac, av);

//	push_pull(1);	// 1=uni-direction 2=bi-direction
}


//
// Request - Reply model
//
void	req_rep(int ac, char *av[])
{
	zmq::context_t context(1);

	zmq::socket_t requester(context, ZMQ_REQ);
	requester.connect("tcp://192.168.1.10:7070");
//	requester.connect("tcp://192.168.1.10:7071");	// 여러 서버로 보낼 때는, N번 보내야 각 서버로 전송됨 
//	requester.connect("tcp://192.168.1.10:7072");
	int nsvr = 1;

	int bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
	requester.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
	requester.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

//	const char *data = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=";
	const char *data = av[1];

	for (int count = 0; count < 1000000; count++)
	{
		char	msg[100] = {0};

		snprintf(msg, sizeof(msg) - 1, "SEND %5d %s", count, data);

		for (int ii = 0; ii < nsvr; ii++)
		{
			s_send(requester, msg);

			string reply = s_recv(requester);

			if (strstr(reply.c_str(), data) == NULL)
				printf("ERROR: REPLY=%s!\n", reply.c_str());

		//	if (count % 10000 == 0)
				cout << "Received reply " << count << " [" << reply << "]" << endl;
		}
	}
}


//
// Push Pull model
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
	sender.connect("tcp://192.168.1.10:5558");

	uint64_t count = 0;
	uint64_t sendsz = 0, recvsz = 0;
	const char *data = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=";

	//  Process tasks forever
	while (1)
	{
		zmq::message_t message;

		//  Send results to sink
		count++;
		s_send(sender, data);

		sendsz += strlen(data);
	//	if (count % 10000 == 0)
			printf("SEND: %lu %.3g Mbytes\n", count, sendsz / 1000000.0);

		if (direc == 2)
		{
			receiver.recv(&message);
			std::string smessage(static_cast<char*>(message.data()), message.size());
			recvsz += smessage.length();
		//	if (count % 10000 == 0)
				printf("RECV: %lu %.3g Mbytes\n", count, recvsz / 1000000.0);
		}
/***
		std::istringstream iss(smessage);
		iss >> workload;

		//  Do the work
		s_sleep(workload);
***/
		usleep(500 * 1000);
	}
}
