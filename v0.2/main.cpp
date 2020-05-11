//
// Usage: tx [-cPORT] maxnode publish_port peer1:port1 [peer2:port2 ...]
//		-cPORT : Client input mode (auto data generation mode disabled)
// ex:
//	tx 4 7000 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7001 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7002 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7003 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//

#include "txcommon.h"


int	_nverifier = MAX_VERIFIER;
int	_automode = 1;		// auto data generation mode (client input disabled)
int	_clientport = DEFAULT_CLIENT_PORT;	

int	_maxnode = 1;		// 나중에 설정으로 뺄 것 
int	_chainport = DEFAULT_CHAIN_PORT;

int	_npeer = 0;
char	_peerlist[MAX_NODE + 1][40] = {0};

safe_queue<txdata_t>	_sendq;		// send queue for publisher
safe_queue<txdata_t>	_recvq;		// receive queue for subscriber
safe_queue<txdata_t>	_veriq;		// receive queue for verifier


void	parse_command_line(int ac, char *av[]);
void	create_main_threads();
void	create_subscriber_threads();
void	create_verifier_threads(int nverifiers);


int	main(int ac, char *av[])
{
	parse_command_line(ac, av);

	_sendq.setmax(100000);
	_recvq.setmax(100000);
	_veriq.setmax(100000);

	create_main_threads();

	create_subscriber_threads();			// SUBSCRIBER

	create_verifier_threads(_nverifier);		// VERIFIER

	// 테스트용 코드이므로..
	while (1)
	{
		sleep(1);
		fflush(stdout);
		fflush(stderr);
	}

	return 0;
}


//
// Parsing command line parameters
//
// Usage: tx maxnode publish_port peer1:port1 [peer2:port2 ...]
//
void	parse_command_line(int ac, char *av[])
{
	int	ii = 0;

	if (ac >= 2 && strncmp(av[1], "-c", 2) == 0)
	{
		_automode = 0;
		_clientport = atoi(&av[1][2]);
		if (_clientport <= 0)
			_clientport = DEFAULT_CLIENT_PORT;
		ac--, av++;
	}
	printf("Auto mode = %d\n", _automode);
	printf("Client port = %d\n", _clientport);

	// 최대 노드 개수 지정 
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		_maxnode = atoi(av[1]);
		if (_maxnode > 100)
			_maxnode = 100;	// 최대 node는 100개로..
		ac--, av++;
	}
	printf("Max node = %d\n", _maxnode);


	// 발송 포트 지정 
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		_chainport = atoi(av[1]);
		if (_chainport <= 10 || _chainport > 65535)
		{
			fprintf(stderr, "ERROR: port range error (1 ~ 65535)\n");
			exit(-1);
		}
		ac--, av++;
	}
	printf("Chain port = %d\n", _chainport);


	// 연결할 peer 지정 (테스트 때는 다이나믹하게 바뀌지 않고 고정으로..)
	for (ii = 1; ii < ac && _npeer < MAX_NODE; ii++)
	{
		if (_npeer > _maxnode)
		{
			fprintf(stderr, "WARNING: Number of peer %d > Max node %d\n", _npeer, _maxnode);
			continue;
		}

		strcpy(_peerlist[_npeer], av[ii]);
		printf("peer[%d] = %s\n", _npeer, _peerlist[_npeer]);
		_npeer++;
	}
	printf("\n\n");
}


void	create_main_threads()
{
	pthread_t thrid;
	int	ret = 0;

	// Send thread
	printf("Create [publisher] thread: sendport=%d\n", _chainport);
	ret = pthread_create(&thrid, NULL, thread_publisher, (void *)&_chainport);
	if (ret < 0)
	{
		perror("thread_publisher() thread creation error");
		exit(-1);
	}
	pthread_detach(thrid);
	sleepms(10);

	// Send test thread
	if (_automode)
	{
		printf("Create [send test] thread\n");
		ret = pthread_create(&thrid, NULL, thread_send_test, NULL);
		if (ret < 0)
		{
			perror("thread_send_test() thread creation error");
			exit(-1);
		}
		pthread_detach(thrid);
		sleepms(10);
	}

	// level db thread
	printf("Create [leveldb] thread\n");
	ret = pthread_create(&thrid, NULL, thread_levledb, (void *)&_chainport);
	if (ret < 0)
	{
		perror("thread_levledb() thread creation error");
		exit(-1);
	}
	pthread_detach(thrid);
	sleepms(10);
}

void	create_subscriber_threads()
{
	pthread_t cthrid, thrid[100];
	int	ret = 0, idx = 0;

	// Client thread
	if (_automode == 0)
	{
		printf("Create [client] thread\n");
		ret = pthread_create(&cthrid, NULL, thread_client, (void *)&_clientport);
		if (ret < 0)
		{
			perror("thread_client() thread creation error");
			exit(-1);
		}
		pthread_detach(cthrid);
		sleepms(10);
	}

	// Subscriber threads
	for (idx = 0; idx < _npeer; idx ++)
	{
		char	*peer = _peerlist[idx];

		char	*tp = strchr(peer, ':');
		if (tp == NULL)
		{
			fprintf(stderr, "ERROR: peer format is IP:PORT. %s skipped\n", peer);
			exit(-1);
		}

		printf("Create subscriber thread [%d]=%s\n", idx, peer);

		// 다수의 노드로 테스트할 때는 자신이 자신의 프로세스에게 발송 요청을 하지 않음.
		if (_maxnode > 1 && atoi(tp+1) == _chainport)
		{
			printf("Peer %s skipped.\n", peer);
			printf("\n");
			continue;
		}

		ret = pthread_create(&thrid[idx], NULL, thread_subscriber, (void *)_peerlist[idx]);
		if (ret < 0)
		{
			perror("thread_subscriber() thread creation error");
			exit(-1);
		}
		pthread_detach(thrid[idx]);
		sleepms(10);
	}
}


void	create_verifier_threads(int nverifiers)
{
	pthread_t thrid[100];
	int	ret = 0, idx = 0;

	// Verifier thread creation
	for (int idx = 0; idx < nverifiers; idx++)
	{
		int	id = idx;
	        ret = pthread_create(&thrid[idx], NULL, thread_verifier, (void *)&id);
                if (ret < 0) {
			perror("thread_verifier() thread creation error");
                        exit(-1);
                }
		pthread_detach(thrid[idx]);
                sleepms(10);
	}
}
