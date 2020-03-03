#include <stdio.h>
#include <pthread.h>

#include "subscribe.h"
#include "publish.h"

int main () {
	
	pthread_t p_thread[5];
	int thr_id;
	int status;	
	int i;

	char info[5][1024];  

	thr_id = pthread_create(&p_thread[0], NULL, publish, 
			(void *)info[0]);
	if (thr_id < 0)
	{
		perror("thread create error : ");
		return 0;
	}


	for (i = 1; i < 4; i ++) {

		thr_id = pthread_create(&p_thread[i], NULL, subscribe, 
				(void *)info[i]);
		if (thr_id < 0)
		{
			perror("thread create error : ");
			return 0;
		}

	}

	pthread_join(p_thread[0], (void **)&status);
	for (i = 1; i < 4; i++) {
		pthread_join(p_thread[i], (void **)&status);
	}

	return 0;
}
