#include <string>
#include "zhelpers.hpp"


using namespace std;


void	req_rep(int ac, char *av[]);
void	push_pull(int direc);


int	main(int ac, char *av[])
{
	req_rep(ac, av);

//	push_pull(1);	// 1=uni-directional  2=bi-directional
}


//
// Request - Reply model
// �ټ��� Client�� send �� �Ʒ��� �ڵ�� ���� receive�ϸ�, 
// ��Ȯ�� �ڽ��� ���� �Ϳ� ���ؼ� ������ �� (queue�� ���� �۵���)
//
void	req_rep(int ac, char *av[])
{
	int	port = 7070;
	char	ip_port[100] = {0};

	if (ac == 2)
	{
		port = atoi(av[1]);
		ac--, av++;
	}

	zmq::context_t context(1);

	zmq::socket_t responder(context, ZMQ_REP);
	sprintf(ip_port, "tcp://*:%d", port);
	responder.bind(ip_port);
	printf("READY: %s\n", ip_port);

	int bufsize = 64 * 1024;	// 1MB ���� 
	responder.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

	bufsize = 64 * 1024;	// 1MB ���� 
	responder.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	int	count = 0;

	while(1)
	{
		char	retbuf[256] = {0};

		//  Wait for next request from client
		string str = s_recv(responder);

		count++;
		snprintf(retbuf, sizeof(retbuf) - 1, "RECV port %d, %5d: %s", port, count, str.c_str());

	//	if (count % 10000 == 0)
			cout << count << " Received request: " << str << endl;

		// Send reply back to client
		s_send(responder, retbuf);

	//	zmq::message_t reply(256);
	//	memcpy(reply.data(), retbuf.c_str(), retbuf.length());
	//	responder.send(reply);
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
	receiver.bind("tcp://*:5558");

	//  Socket to send messages to
	zmq::socket_t sender(context, ZMQ_PUSH);
	if (direc == 2)		// bi-directional
	{
		sender.connect("tcp://192.168.1.10:5557");
	}

	uint64_t count = 0;
	uint64_t recvsz = 0, sendsz = 0;

	//  Process tasks forever
	while (1)
	{
		zmq::message_t message;

		count++;
		receiver.recv(&message);
		std::string smessage(static_cast<char*>(message.data()), message.size());

		recvsz += smessage.length();
	//	if (count % 10000 == 0)
			printf("RECV: %lu %.3g Mbytes\n", count, recvsz / 1000000.0);

		//  Send results to sink
		if (direc == 2)
		{
			sender.send(message);
		//	s_send(sender, smessage);

			sendsz += message.size();
		//	if (count % 10000 == 0)
				printf("SEND: %lu %.3g Mbytes\n", count, sendsz / 1000000.0);
		}
		usleep(500 * 1000);
	}
}
