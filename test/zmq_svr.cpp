#include <string>
#include "zhelpers.hpp"


using namespace std;


void	req_rep(int port);


int	main(int ac, char *av[])
{
	int	port = 7070;

	if (ac == 2)
		port = atoi(av[1]);

	req_rep(port);
}


//
// Request - Reply model
//
void	req_rep(int port)
{
	char	ip_port[100] = {0};

	zmq::context_t context(1);

	zmq::socket_t responder(context, ZMQ_REP);
	sprintf(ip_port, "tcp://*:%d", port);
	responder.bind(ip_port);
	printf("READY: %s\n", ip_port);

	int bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
	responder.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

	bufsize = 1 * 1024 * 1024;	// 1MB 버퍼 
	responder.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	int	count = 0;

	while(1)
	{
		char	retbuf[256] = {0};

		//  Wait for next request from client
		string str = s_recv(responder);

		count++;
		snprintf(retbuf, sizeof(retbuf) - 1, "RECV port %d, %5d: %s\n", port, count, str.c_str());

	//	if (count % 10000 == 0)
			cout << count << " Received request: " << str << endl;

		// Send reply back to client
		s_send(responder, retbuf);

	//	zmq::message_t reply(256);
	//	memcpy(reply.data(), retbuf.c_str(), retbuf.length());
	//	responder.send(reply);
	}
}
