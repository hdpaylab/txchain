#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include "xsub.h"
#include "xpub.h"


int	main(int ac, char *av[])
{
	
	pthread_t thrid[5];
	int	ret = 0;
	int	ii = 0;
	int	maxnode = 1;	// ���߿� �������� �� �� 
	char	info[5][256];

	if (ac == 2 && atoi(av[1]) > 1)
	{
		maxnode = atoi(av[1]);
		if (maxnode > 10)
			maxnode = 10;	// ��а� �ִ� node�� 10����..
	}

	// �߽��� 
	ret = pthread_create(&thrid[0], NULL, publish, (void *)info[0]);
	if (ret < 0)
	{
		perror("thread create error : ");
		return 0;
	}


	// ������ 
	for (ii = 1; ii <= maxnode; ii ++)
	{
		ret = pthread_create(&thrid[ii], NULL, subscribe, 
				(void *)info[ii]);
		if (ret < 0)
		{
			perror("thread create error : ");
			return 0;
		}

	}

	for (ii = 0; ii <= maxnode; ii++) {
		pthread_detach(thrid[ii]);
	}

	while (1)
	{
		sleep(1);
		fflush(stdout);
	}

	return 0;
}
