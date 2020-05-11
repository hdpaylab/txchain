#ifndef __COMMON_H
#define __COMMON_H


#include <zmq.hpp>
#include <iostream>
#include <string>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <keys/hs_keys_wrapper.h>	// xparams.h
#include <sys/time.h>
#include <leveldb/c.h>			// xleveldb.cpp


using namespace std;


#include "zhelpers.hpp"
#include "safe_queue.h"


//#define DEBUG		1		// for debugging
#define DEBUG_SLEEP_MS	100
#define MAX_TEST_NUM_TX	1000000		// tx count for test..



//#define TXCHAIN_VERIFY_MODEL_MSGQ	1
#define TXCHAIN_VERIFY_MODEL_QUEUE	3


#define MAX_NODE		100

#define MAX_VERIFIER		4		// default thread 개수 
#define DEFAULT_CLIENT_PORT	6999		// default client management port
#define DEFAULT_CHAIN_PORT	7000		// default chain management port

#define MAX_SEND_QUEUE_SIZE	100		// send queue for publisher
#define MAX_RECV_QUEUE_SIZE	100		// receive queue for subscriber
#define MAX_VERIFY_QUEUE_SIZE	100		// receive queue for verifier

#define sleepms(n)	usleep(n * 1000)	// ms 단위 sleep
#define MAX_SEQ		0xFFFFFFFF		// 마지막 

#define ZMQ_FILTER	"!@#$"			// ZMQ 구분자 
#define TX_DELIM	'|'			// TX delimiter


typedef unsigned char	uchar;

// msg 데이터 전송 테스트용 
typedef struct {
	char	*pubkey;
	char	*message;
	char	*signature;
	int	verified;
}	txmsg_t;

enum {
	TXCHAIN_STATUS_ERROR		= 0xFFFFFFFF,
	TXCHAIN_STATUS_VALID		= 0x61206120,
	TXCHAIN_STATUS_EMPTY		= 0x00000000,
	TXCHAIN_STATUS_READY		= 0x00010000,
	TXCHAIN_STATUS_RECV		= 0x00020000,
	TXCHAIN_STATUS_VERI		= 0x00040000,
	TXCHAIN_STATUS_VERI_REQ		= 0x00080000,
	TXCHAIN_STATUS_VERI_RESULT	= 0x00100000,
};


typedef struct {
	string		data;
	uint32_t	seq;		// TX sequence
	uint32_t	valid;		// Valid TX mark
	int		verified;	// 0=fail 1=success -1=none 
					// WARNING: DO NOT MOVE status position!
	uint32_t	status;		// see above TXCHAIN_STATUS_xxx
}	txdata_t;


extern	int	_nverifier;	// current number of verifier threads

extern	safe_queue<txdata_t>	_sendq;		// send queue for publisher
extern	safe_queue<txdata_t>	_recvq;		// receive queue for subscriber
extern	safe_queue<txdata_t>	_veriq;		// receive queue for verifier


////////////////////////////////////////////////////////////////////////////////

Params_type_t paramsget(const string& Path);	// params.cpp

void	*thread_publisher(void *info_p);	// pub.cpp
void	*thread_send_test(void *info_p);	// main.cpp
void	*thread_subscriber(void *info_p);	// sub.cpp
void	*thread_client(void *info_p);		// sub.cpp
void	*thread_verifier(void *info_p);		// verify.cpp
void	*thread_levledb(void *info_p);		// leveldb.cpp

double	xgetclock();				// util.cpp


////////////////////////////////////////////////////////////////////////////////
#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

#define SUBSCRIBER_MSGQ_ID	1234
#define VERIFIER_MSGQ_ID	1235

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ


#endif	// __COMMON_H
