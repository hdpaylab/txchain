#include <stdio.h>
#include <pthread.h>

#include "subscribe.h"

int main () {
	
	pthread_t p_thread[4];
	int thr_id;
	int status;	
	int i;

	char info[4][1024];  

	for (i = 0; i < 4; i ++) {

		thr_id = pthread_create(&p_thread[i], NULL, subscribe, 
				(void *)info[i]);
		if (thr_id < 0)
		{
			perror("thread create error : ");
			return 0;
		}

	}
	for (i = 0; i < 4; i++) {
		pthread_join(p_thread[i], (void **)&status);
	}

	return 0;
}
