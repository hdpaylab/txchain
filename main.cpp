//
// Usage: tx maxnode publish_port peer1:port1 [peer2:port2 ...]
// ex:
//	tx 4 7000 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7001 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7002 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//	tx 4 7003 192.168.1.10:7000 192.168.1.10:7001 192.168.1.10:7002 192.168.1.10:7003
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "xsub.h"
#include "xpub.h"


#define MAX_NODE	100


int	maxnode = 1;		// 나중에 설정으로 뺄 것 
int	sendport = 7000;
int	npeer = 0;
char	peers[MAX_NODE][40] = {0};


void	load_config(int ac, char *av[]);


int	main(int ac, char *av[])
{
	
	pthread_t thrid[5];
	int	ret = 0;
	int	ii = 0;


	load_config(ac, av);


	// 발신자 thread
	printf("Create publisher sendport=%d\n", sendport);
	ret = pthread_create(&thrid[0], NULL, thread_publisher, (void *)&sendport);
	if (ret < 0)
	{
		perror("thread create error : ");
		return 0;
	}
	usleep(100 * 1000);


	// 수신자 
	for (ii = 1; ii <= npeer; ii ++)
	{
		char	*peer = peers[ii-1];

		printf("Create subscriber [%d]=%s\n", ii - 1, peer);

		char	*tp = strchr(peer, ':');
		if (tp == NULL)
		{
			fprintf(stderr, "ERROR: peer format is IP:PORT. %s skipped\n", av[ii]);
			exit(-1);
		}

		// 다수의 노드로 테스트할 때는 자신이 자신의 프로세스에게 발송 요청을 하지 않음.
		if (maxnode > 1 && atoi(tp+1) == sendport)
		{
			printf("Peer %s skipped.\n", peer);
			printf("\n");
			continue;
		}

		ret = pthread_create(&thrid[ii], NULL, thread_subscriber, (void *)peers[ii-1]);
		if (ret < 0)
		{
			perror("thread create error : ");
			return 0;
		}
		usleep(10 * 1000);

	}

	for (ii = 0; ii <= npeer; ii++) {
		pthread_detach(thrid[ii]);
	}

	while (1)
	{
		sleep(1);
		fflush(stdout);
	}

	return 0;
}


void	load_config(int ac, char *av[])
{
	int	ii = 0;

	// 최대 노드 개수 지정 
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		maxnode = atoi(av[1]);
		if (maxnode > 100)
			maxnode = 100;	// 최대 node는 100개로..
		ac--, av++;
	}
	printf("Max node = %d\n", maxnode);


	// 발송 포트 지정 
	if (ac >= 2 && atoi(av[1]) > 0)
	{
		sendport = atoi(av[1]);
		if (sendport > 65535)
		{
			fprintf(stderr, "ERROR: port range error (1 ~ 65535)\n");
			exit(-1);
		}
		ac--, av++;
	}
	printf("Send port = %d\n", sendport);


	// 연결할 peer 지정 (테스트 때는 다이나믹하게 바뀌지 않고 고정으로..)
	for (ii = 1; ii < ac && npeer < MAX_NODE; ii++)
	{
		if (npeer > maxnode)
		{
			fprintf(stderr, "WARNING: Number of peer %d > Max node %d\n", npeer, maxnode);
			continue;
		}

		strcpy(peers[npeer], av[ii]);
		printf("peer[%d] = %s\n", npeer, peers[npeer]);
		npeer++;
	}
	printf("\n\n");
}
