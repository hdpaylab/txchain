//
// Usage: tx maxnode publish_port peer1:port1 [peer2:port2 ...]
// ex:
//	tx 4 7000 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7001 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7002 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7003 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//

#include "txcommon.h"


int	_nverifier = MAX_VERIFIER;
	

vector<txdata_t> _txv(MAX_VECTOR_SIZE);
int	_push_count = 0, _pop_count = 0;


int	_maxnode = 1;		// ���߿� �������� �� �� 
int	_sendport = 7000;

int	_npeer = 0;
char	_peerlist[MAX_NODE + 1][40] = {0};


void	parse_command_line(int ac, char *av[]);


int	main(int ac, char *av[])
{
	
	pthread_t thrid[6];
	int	ret = 0;
	int	ii = 0;


	parse_command_line(ac, av);


	// �߽��� thread
	printf("Create publisher sendport=%d\n", _sendport);
	ret = pthread_create(&thrid[0], NULL, thread_publisher, (void *)&_sendport);
	if (ret < 0)
	{
		perror("thread create error : ");
		return 0;
	}
	usleep(100 * 1000);


	// ������ 
	for (ii = 1; ii <= _npeer; ii ++)
	{
		char	*peer = _peerlist[ii-1];

		printf("Create subscriber [%d]=%s\n", ii - 1, peer);

		char	*tp = strchr(peer, ':');
		if (tp == NULL)
		{
			fprintf(stderr, "ERROR: peer format is IP:PORT. %s skipped\n", av[ii]);
			exit(-1);
		}

		// �ټ��� ���� �׽�Ʈ�� ���� �ڽ��� �ڽ��� ���μ������� �߼� ��û�� ���� ����.
		if (_maxnode > 1 && atoi(tp+1) == _sendport)
		{
			printf("Peer %s skipped.\n", peer);
			printf("\n");
			continue;
		}

		ret = pthread_create(&thrid[ii], NULL, thread_subscriber, (void *)_peerlist[ii-1]);
		if (ret < 0)
		{
			perror("thread create error : ");
			return 0;
		}
		usleep(10 * 1000);

	}

	// level db thread
	ret = pthread_create(&thrid[ii], NULL,
			thread_levledb, (void *)&_sendport);
	if (ret < 0)
	{
		perror("thread create error : ");
		return 0;
	}
	usleep(10 * 1000);

	for (ii = 0; ii <= _npeer; ii++) {
		pthread_detach(thrid[ii]);
	}
	// _npeer+1 = leveldb thread
	pthread_detach(thrid[ii]);

	while (1)
	{
		sleep(1);
		fflush(stdout);
		fflush(stderr);
	}

	return 0;
}


void	parse_command_line(int ac, char *av[])
{
	int	ii = 0;

	// �ִ� ��� ���� ���� 
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		_maxnode = atoi(av[1]);
		if (_maxnode > 100)
			_maxnode = 100;	// �ִ� node�� 100����..
		ac--, av++;
	}
	printf("Max node = %d\n", _maxnode);


	// �߼� ��Ʈ ���� 
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		_sendport = atoi(av[1]);
		if (_sendport <= 10 || _sendport > 65535)
		{
			fprintf(stderr, "ERROR: port range error (1 ~ 65535)\n");
			exit(-1);
		}
		ac--, av++;
	}
	printf("Send port = %d\n", _sendport);


	// ������ peer ���� (�׽�Ʈ ���� ���̳����ϰ� �ٲ��� �ʰ� ��������..)
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
