#include <string>
#include "zhelpers.hpp"


using namespace std;


void	req_rep();


int	main(int argc, char *argv[])
{
	req_rep();
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
void	push_pull()
{
	zmq::context_t context(1);

	zmq::socket_t requester(context, ZMQ_REQ);
	requester.connect("tcp://192.168.1.10:7070");

	int bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
	requester.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
	requester.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

	string msg = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=";

	for (int count = 0; count < 100000 ; count++)
	{
		s_send(requester, msg);

		string reply = s_recv(requester);

		if (count % 10000 == 0)
			cout << "Received reply " << count << " [" << reply << "]" << endl;
	}
}
