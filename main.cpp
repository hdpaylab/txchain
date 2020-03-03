#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "xsub.h"
#include "xpub.h"

int main (int ac, char *av[])
{
	
	pthread_t p_thread[5];
	int thr_id;
	int status;	
	int i;
	char info[5][1024];  

	thr_id = pthread_create(&p_thread[0], NULL, publish, (void *)info[0]);
	if (thr_id < 0)
	{
		perror("thread create error : ");
		return 0;
	}


//	for (i = 1; i < 4; i ++) {
	for (i = 1; i < 2; i ++)
	{
		thr_id = pthread_create(&p_thread[i], NULL, subscribe, 
				(void *)info[i]);
		if (thr_id < 0)
		{
			perror("thread create error : ");
			return 0;
		}

	}

	for (i = 0; i < 4; i++) {
		pthread_detach(p_thread[i]);
	}

	while (1)
	{
		sleep(1);
	}

	return 0;
}
