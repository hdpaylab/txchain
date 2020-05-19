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
#include "../v0.2/txcommon.h"
#include <queue>
#include "safe_queue.h"


void	test_queue();
void	mutex_test_vector();
void	mutex_test_queue();
void	memory_test();

typedef struct {
	string data;
	uint32_t	seq;		// TX sequence
	uint32_t	valid;		// Valid TX mark
	int		verified;	// 0=fail 1=success -1=none 
					// WARNING: DO NOT MOVE status position!
	uint32_t	status;		// see above TXCHAIN_STATUS_xxx
} mydata_t;

safe_queue<mydata_t>	_safeq;		// send queue for publisher


int	main(int ac, char *av[])
{
	if (ac == 1 && av[0] == 0)
		ac = ac;

	const char *aa = "AB";

	string ss(aa, 2);
	printf("LEN=%ld\n", ss.length());

	const char *sp = ss.c_str();
	printf("sp=%s sp[2]=0x%02X\n", sp, sp[2]);

//	memory_test();

//	mutex_test_queue();

//	mutex_test_vector();

	test_queue();
}


////////////////////////////////////////////////////////////////////////////////
//
// Queue test
//

void	*thread_queue_send(void *arg)
{
	for (int ii = 0; ii < 10000000; ii++)
	{
		mydata_t mydata;

		string data = "H31jhfsdj12j3h214g2134h123kj4|Hdac Technology, 잘 가는지 검사하는 것임23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789|000000afasfsadfksadfsakdjfsajKJHSAFKDJDLKASJDLAJDALDJLASJDA";

		mydata.data = data;

		_safeq.push(mydata);
	}
	return NULL;
}


void	*thread_queue_recv(void *arg)
{
	for (int ii = 0; ii < 2500000; ii++)
	{
		mydata_t mydata = _safeq.pop();

		if (ii % 10000 == 0)
			printf("Queue test: count=%d data=%s\n", ii, mydata.data.c_str());
	}
	return NULL;
}


void	test_queue()
{
	pthread_t tidsend, tidrecv[4];

	int ret = pthread_create(&tidsend, NULL, thread_queue_send, NULL);
	assert(ret == 0);

	ret = pthread_create(&tidrecv[0], NULL, thread_queue_recv, NULL);
	ret = pthread_create(&tidrecv[1], NULL, thread_queue_recv, NULL);
	ret = pthread_create(&tidrecv[2], NULL, thread_queue_recv, NULL);
	ret = pthread_create(&tidrecv[3], NULL, thread_queue_recv, NULL);

	printf("Start queue test...\n\n");
	pthread_join(tidsend, NULL);
	pthread_join(tidrecv[0], NULL);
	pthread_join(tidrecv[1], NULL);
	pthread_join(tidrecv[2], NULL);
	pthread_join(tidrecv[3], NULL);
}


////////////////////////////////////////////////////////////////////////////////
//
// Mutex test with vector
//
typedef struct {
	char	pubkey[64];
	char	message[256];
	char	sign[128];
	int	verified;
	int	status;		// 1=ready 2=processed
}	mytxdata_t;

#define STATUS_EMPTY		0
#define STATUS_READY		1
#define STATUS_PROCESSED	2

vector<mytxdata_t> myv(10000);
int	nthread = 4;
int	pop_count = 0;
int	push_count = 0;


void	*thread_mutex_test_vector(void *arg)
{
	int	id = *(int *)arg;
	int	loop = 250000, idx = 0;
	int	count = 0;

	for (int ii = 0; ii < loop; )
	{
		idx = (ii * nthread + id) %  myv.size();	// 자신의 위치 데이터만 처리

		if (myv[idx].status != STATUS_READY)
		{
			usleep(10);
			continue;
		}

		mytxdata_t tx;

		tx = myv[idx];
	//	printf("THR%d: [%d]: count=%d pubkey=%s \n",	//message=%s sign=%s\n",
	//		id, idx, count, myv[idx].pubkey);	//, myv[idx].message, myv[idx].sign);

	//	if (pop_count % 100000 == 0)
	//		printf("THR%d : count=%d size=%ld pubkey=%s message=%s sign=%s\n",
	//			id, pop_count, myv.size(), myv[idx].pubkey, myv[idx].message, myv[idx].sign);
		myv[idx].verified = 1;
		myv[idx].status = STATUS_PROCESSED;

		pop_count++;
		count++;
		ii++;		// 1 processed
	//	usleep(100);
	}

	printf("\n=====THR%d: END count=%d\n", id, count);

	pthread_exit(NULL);

	return arg;
}

//
// pthread_mutex 테스트 코드
//
void	mutex_test_vector()
{
	int	id[10] = {0};
	int	count = 0;
	int	loop = 1000000;
	double	tmstart = 0, tmend = 0;
	pthread_t	tid[10];

	for (int ii = 0; ii < (ssize_t)myv.size(); ii++)
	{
		myv[ii].verified = -1;
		myv[ii].status = STATUS_EMPTY;
	}

	for (int ii = 0; ii < nthread; ii++)
	{
		id[ii] = ii; 
		int ret = pthread_create(&tid[ii], NULL, thread_mutex_test_vector, &id[ii]);
		pthread_detach(tid[ii]);
		assert(ret >= 0);
	}
	usleep(100 * 1000);

	tmstart = xgetclock();

	const char *pubkey = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";
	const char *signature = "ILM7liyTJJ+yrvXhPX4bFaABRKKPuW/EPGDTNL7mRdyaL6TZGRxA0cBVUWWeWtyrFc20YJYiJK7V2DJ7eqQmzzo=";

	for (int ii = 0; ii < loop; )
	{
		int	idx = ii % myv.size();
		mytxdata_t	tx;

		// 다음 위치 준비가 되면..
		if (myv[idx].status != 0 && myv[idx].status != STATUS_PROCESSED)
		{
			usleep(10);
		}

		memset(&tx, 0, sizeof(tx));
		snprintf(tx.message, sizeof(tx.message), "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 %d", ii);
		strcpy(tx.pubkey, pubkey);
		strcpy(tx.sign, signature);
		tx.verified = -1;
		tx.status = STATUS_READY;

		myv[idx] = tx;
	//	printf("Add: count=%d size=%ld\n", push_count, myv.size());

		push_count++;
		count++;

		if (push_count % 200000 == 0)
			printf("Add: count=%d size=%ld\n", push_count, myv.size());
		ii++;

	//	usleep(100);
	}

	tmend = xgetclock();

	printf("\n=====ITEM=%d PROCESS=%d\n\n", push_count, pop_count);

	printf("End pthread_mutex VECTOR test ok: %.3f sec, count=%d / %.1f/sec\n",
		tmend - tmstart, count, count / (tmend - tmstart));
	sleep(1);
}


////////////////////////////////////////////////////////////////////////////////
//
// Mutex test with queue
//
queue<string> myq;
pthread_mutex_t mtx;


void	*thread_mutex_test_queue(void *arg)
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
				printf("THR%d : count=%d size=%ld data=%s\n", id, pop_count, myq.size(), data.c_str());
			ii++;		// 1 processed
		}
		pthread_mutex_unlock(&mtx);
	}

	printf("\n=====THR%d: END count=%d\n", id, count);

	return arg;
}

//
// pthread_mutex 테스트 코드
//
void	mutex_test_queue()
{
	int	id[10] = {0};
	int	count = 0;
	int	loop = 1000000;
	double	tmstart = 0, tmend = 0;
	pthread_t	tid[10];

	pthread_mutex_init(&mtx, NULL);

	for (int ii = 0; ii < nthread; ii++)
	{
		id[ii] = ii; 
		int ret = pthread_create(&tid[ii], NULL, thread_mutex_test_queue, &id[ii]);
		pthread_detach(tid[ii]);
		assert(ret >= 0);
	}
	usleep(100 * 1000);

	tmstart = xgetclock();

	for (int ii = 0; ii < loop; )
	{
		char	data[512];

		if (myq.size() > 10000)
		{
			if (myq.size() % 1000 == 0)
				printf("Queue: count=%d size=%ld SLEEP...\n", push_count, myq.size());
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
			printf("Queue: count=%d size=%ld data=%s\n", push_count, myq.size(), data);

		ii++;
	}

	tmend = xgetclock();

	pthread_mutex_destroy(&mtx);

	printf("\n=====PUSH=%d POP=%d\n\n", push_count, pop_count);

	printf("End pthread_mutex QUEUE test ok: %.3f sec, count=%d / %.1f/sec\n",
		tmend - tmstart, count, count / (tmend - tmstart));
}


////////////////////////////////////////////////////////////////////////////////
//
// memory test
//
void	memory_test()
{
	int	count = 0;
	int	loop = 1000000;
	double	tmstart = 0, tmend = 0;
	char	*tp = NULL;
	const char *user_data = "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 ";
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

