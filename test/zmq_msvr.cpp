//
// Multithreaded Hello World server in C
//

#include <pthread.h>
#include <unistd.h>
#include <cassert>
#include <string>
#include <iostream>
#include <zmq.hpp>


void	*worker_routine (void *arg)
{
	int	count = 0;

	zmq::context_t *context = (zmq::context_t *) arg;

	zmq::socket_t socket (*context, ZMQ_REP);
	socket.connect ("inproc://workers");

	while (true)
	{
		char	retbuf[256] = {0};

		//  Wait for next request from client
		zmq::message_t request;
		socket.recv (&request);
		std::cout << "Received request: [" << (char*) request.data() << "]" << std::endl;

		count++;
		snprintf(retbuf, sizeof(retbuf) - 1, "%5d: %s", count, (char *)request.data());

		if (strstr(retbuf, "1111"))
			usleep(1000 * 1000);
		else
			usleep(100 * 1000);

		//  Send reply back to client
		zmq::message_t reply(strlen(retbuf) + 1);
		memcpy((void *)reply.data(), retbuf, strlen(retbuf) + 1);
		socket.send(reply);
	}
	return (NULL);
}


int	main(int ac, char *av[])
{
	int	port = 7070;
	char	ip_port[100] = {0};

	if (ac == 2)
	{
		port = atoi(av[1]);
		ac--, av++;
	}

	//  Prepare our context and sockets
	zmq::context_t context (1);
	zmq::socket_t clients (context, ZMQ_ROUTER);
	sprintf(ip_port, "tcp://*:%d", port);
	clients.bind(ip_port);
	printf("READY: %s\n", ip_port);

	zmq::socket_t workers (context, ZMQ_DEALER);
	workers.bind ("inproc://workers");

	//  Launch pool of worker threads
	for (int thread_nbr = 0; thread_nbr != 5; thread_nbr++)
	{
		pthread_t worker;
		pthread_create (&worker, NULL, worker_routine, (void *) &context);
	}

	//  Connect work threads to client threads via a queue
	zmq::proxy (static_cast<void*>(clients), static_cast<void*>(workers), nullptr);

	return 0;
}
