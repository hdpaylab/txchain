#include "txcommon.h"


pthread_mutex_t _sendmtx;	// for _sendq
pthread_mutex_t _recvmtx;	// for _recvq
pthread_mutex_t _vermtx;	// for _verq

int	_mtx_initialized = 0;

queue<txdata_t>	_sendq;		// send queue for publisher
queue<txdata_t>	_recvq;		// receive queue for subscriber
queue<txdata_t>	_verq;		// receive queue for verifier

int	_push_count = 0, _pop_count = 0;


int	send_queue_push(txdata_t data)
{
	return common_queue_push("SQ_PUSH", _sendq, data, &_sendmtx, MAX_SEND_QUEUE_SIZE);
}


txdata_t send_queue_pop()
{
	return common_queue_pop("SQ_POP ", _sendq, &_sendmtx);
}


int	recv_queue_push(txdata_t data)
{
	return common_queue_push("RQ_PUSH", _recvq, data, &_recvmtx, MAX_RECV_QUEUE_SIZE);
}


txdata_t recv_queue_pop()
{
	return common_queue_pop("RQ_POP ", _recvq, &_recvmtx);
}


int	verify_queue_push(txdata_t data)
{
	return common_queue_push("VQ_PUSH", _verq, data, &_vermtx, MAX_RECV_QUEUE_SIZE);
}


txdata_t verify_queue_pop()
{
	return common_queue_pop("VQ_POP ", _verq, &_vermtx);
}


int	common_queue_push(const char *name, queue<txdata_t>& myq, txdata_t data, pthread_mutex_t *mtx, int max_queue_size)
{
	int	loop = 0;

	if (_mtx_initialized == 0)
	{
		pthread_mutex_init(&_sendmtx, NULL);
		pthread_mutex_init(&_recvmtx, NULL);
		pthread_mutex_init(&_vermtx, NULL);
		_mtx_initialized = 1;
	}

	while ((int)myq.size() >= max_queue_size)
	{	
		loop++;
		if (loop % 10000 == 0)
			printf("%s: size=%ld max=%d waiting...\n",
				name, myq.size(), max_queue_size);
		usleep(10);
	}

	pthread_mutex_lock(mtx);

	myq.push(data);
	_push_count++;
#ifdef DEBUG
	usleep(100 * 1000);
#else
	if (_push_count % 100000 == 0)
#endif
		printf("%s: count=%d size=%ld data=%s\n", 
			name, _push_count, myq.size(), data.data.c_str());

	pthread_mutex_unlock(mtx);

	return 1;
}


txdata_t common_queue_pop(const char *name, queue<txdata_t>& myq, pthread_mutex_t *mtx)
{
	txdata_t data;

	if (_mtx_initialized == 0)
	{
		pthread_mutex_init(&_sendmtx, NULL);
		pthread_mutex_init(&_recvmtx, NULL);
		pthread_mutex_init(&_vermtx, NULL);
		_mtx_initialized = 1;
	}

	// 데이터 들어올 때까지 대기 
	while ((int)myq.size() <= 0)
	{
		usleep(10);
	}

	pthread_mutex_lock(mtx);

	if (myq.size() > 0)
	{
		data = myq.front();
		myq.pop();
		_pop_count++;
#ifdef DEBUG
	usleep(100 * 1000);
#else
		if (_pop_count % 100000 == 0)
#endif
			printf("%s: count=%d size=%ld data=%s\n", 
				name, _pop_count, myq.size(), data.data.c_str());
	}

	pthread_mutex_unlock(mtx);

	return data;
}
