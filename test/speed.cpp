//
// calloc() : 1,000,000 / 0.0001 sec	@100억/sec
// strcpy   : 1,000,000 / 0.17 sec	@588만/sec
// memcpy() : 1,000,000 / 0.17 sec	@588만/sec
// memset() : 1,000,000 / 0.02 sec	@5000만/sec
// assert() : 1,000,000 / 0.164 sec	@600a만/sec
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "../txcommon.h"
#include <queue>


void	mutex_test();
void	memory_test();


int	main(int ac, char *av[])
{
	if (ac == 1 && av[0] == 0)
		ac = ac;

//	memory_test();

	mutex_test();
}

const char *user_data = "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 ";
queue<string> myq;
pthread_mutex_t mtx;
int	pop_count = 0;
int	push_count = 0;


void	*thread_mutex_test(void *arg)
{
	int	id = *(int *)arg;
	int	loop = 250000;
	int	count = 0;

	for (int ii = 0; ii < loop; )
	{
		if (myq.size() <= 0)
		{
			usleep(10);
			continue;
		}
		pthread_mutex_lock(&mtx);
		if (myq.size() > 0)
		{
			string data = myq.front();
			myq.pop();
			pop_count++;
			count++;
			if (pop_count % 100000 == 0)
				printf("THR%d : count=%d size=%d data=%s\n", id, pop_count, myq.size(), data.c_str());
			ii++;		// 1 processed
		}
		pthread_mutex_unlock(&mtx);
	}

	printf("\n=====THR%d: END count=%d\n", id, count);

	return arg;
}

void	mutex_test()
{
	int	id[10] = {0};
	int	count = 0;
	int	loop = 1000000;
	double	tmstart = 0, tmend = 0;
	pthread_t	tid[10];

	pthread_mutex_init(&mtx, NULL);

	for (int ii = 0; ii < 4; ii++)
	{
		id[ii] = ii; 
		int ret = pthread_create(&tid[ii], NULL, thread_mutex_test, &id[ii]);
		pthread_detach(tid[ii]);
		assert(ret >= 0);
	}
	usleep(100 * 1000);

	tmstart = xgetclock();

	for (int ii = 0; ii < loop; ii++)
	{
		char	data[512];

		if (myq.size() > 10000)
		{
			if (myq.size() % 1000 == 0)
				printf("Queue: count=%d size=%d \n", push_count, myq.size());
			usleep(10);
		}

		snprintf(data, sizeof(data), "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 %d", ii);

		pthread_mutex_lock(&mtx);
		myq.push(data);
		push_count++;
		count++;
		pthread_mutex_unlock(&mtx);

	//	usleep(100 * 1000);
		if (push_count % 200000 == 0)
			printf("Queue: count=%d size=%d data=%s\n", push_count, myq.size(), data);
	}

	tmend = xgetclock();

	pthread_mutex_destroy(&mtx);

	printf("\n=====PUSH=%d POP=%d\n\n", push_count, pop_count);

	printf("pthread_mutex test ok: %.3f sec, count=%d / %.1f/sec\n",
		tmend - tmstart, count, count / (tmend - tmstart));
}


void	memory_test()
{
	int	count = 0;
	int	loop = 1000000;
	double	tmstart = 0, tmend = 0;
	char	*tp = NULL;
	const char *sp = user_data;
	int	len = strlen(sp);

	printf("Start calloc...\n");

	tmstart = xgetclock();

	for (int ii = 0; ii < loop; ii++)
	{
		tp = (char *) calloc(1, 256);
	//	assert(tp != NULL);
		*tp = 0;
	//	memset(tp, 0, 256);
	//	strcpy(tp, sp);
		memcpy(tp, sp, len);
		if (ii % 100000 == 0)
			printf("calloc() %d\n", ii);
		count++;
	} 

	tmend = xgetclock();

	printf("End memory test ok: count=%d / %.1f/sec\n",
		count, count / (tmend - tmstart));
}

