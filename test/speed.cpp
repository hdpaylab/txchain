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


void	mutex_test();
void	memory_test();


main()
{
//	memory_test();

	mutex_test();
}


void	mutex_test()
{
	int	count = 0;
	int	loop = 10000000;
	double	tmstart = 0, tmend = 0;
	pthread_mutex_t mtx;

	pthread_mutex_init(&mtx, NULL);

	tmstart = xgetclock();

	for (int ii = 0; ii < loop; ii++)
	{
		pthread_mutex_lock(&mtx);
		count++;
		pthread_mutex_unlock(&mtx);
	}

	tmend = xgetclock();

	pthread_mutex_destroy(&mtx);

	printf("pthread_mutex test ok: count=%d / %.1f/sec\n",
		count, count / (tmend - tmstart));
}


void	memory_test()
{
	int	count = 0;
	int	loop = 1000000;
	double	tmstart = 0, tmend = 0;
	char	*tp = NULL;
	char	*sp = "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 ";
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
		memcpy(tp, sp, strlen(sp));
		if (ii % 100000 == 0)
			printf("calloc() %d\n", ii);
		count++;
	} 

	tmend = xgetclock();

	printf("End memory test ok: count=%d / %.1f/sec\n",
		count, count / (tmend - tmstart));
}

