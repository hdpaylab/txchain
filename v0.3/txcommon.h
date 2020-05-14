#ifndef __COMMON_H
#define __COMMON_H


#include <zmq.hpp>
#include <iostream>
#include <string>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <keys/hs_keys_wrapper.h>	// xparams.h
#include <structs/hashes.h>		// CSHA256()
#include <utils/utilstrencodings.h>	// HexStr()
#include <sys/time.h>
#include <leveldb/c.h>			// xleveldb.cpp


using namespace std;


#include "lib.h"
#include "tx.h"


#define DEBUG		1		// for debugging
#define DEBUG_SLEEP_MS	1000
#define MAX_TEST_NUM_TX	1000000		// tx count for test..



#define TXCHAIN_VERIFY_MODEL_QUEUE	3


#define MAX_NODE		100

#define MAX_VERIFIER		4		// number of verifiers
#define DEFAULT_CLIENT_PORT	6999		// default client management port
#define DEFAULT_CHAIN_PORT	7000		// default chain management port

#define MAX_SEND_QUEUE_SIZE	100		// send queue for publisher
#define MAX_RECV_QUEUE_SIZE	100		// receive queue for subscriber
#define MAX_VERIFY_QUEUE_SIZE	100		// receive queue for verifier

#define sleepms(n)	usleep(n * 1000)	// ms unit sleep
#define MAX_SEQ		0xFFFFFFFF		// sequence

#define ZMQ_FILTER	"!@#$"			// ZMQ delimiter
#define TX_DELIM	'|'			// TX delimiter


typedef unsigned char	uchar;


// msg 데이터 전송 테스트용 
typedef struct {
	char	*address;
	char	*message;
	char	*signature;
	int	verified;
}	txmsg_t;


typedef struct {
	string		data;		// for sign (include tx_sign_t)
	tx_sign_t	sign;		// sign 
	string		txid;		// sha256(sign)

	uint32_t	seq;		// TX sequence
	uint32_t	valid;		// 0=invalid 1=valid -1=none

	uint32_t        nverified;
	uint32_t	status;		// STAT_xxx
}	txdata_t;


// main.cpp
extern	int	_nverifier;	// current number of verifier threads

extern	safe_queue<txdata_t>	_sendq;		// send queue for publisher
extern	safe_queue<txdata_t>	_verifyq;	// receive queue for subscriber
extern	safe_queue<txdata_t>	_mempoolq;	// receive queue for verifier
extern	safe_queue<txdata_t>	_resultq;	// mempool queue (verification reply)

void	*thread_publisher(void *info_p);	// pub.cpp
void	*thread_send_test(void *info_p);	// main.cpp
void	*thread_subscriber(void *info_p);	// sub.cpp
void	*thread_client(void *info_p);		// sub.cpp
void	*thread_verifier(void *info_p);		// verify.cpp
void	*thread_levledb(void *info_p);		// leveldb.cpp


// ldbio.cpp
extern	leveldb	_systemdb;	// 
extern	leveldb	_walletdb;	// 


// common.cpp
const char *get_type_name(int type);
const char *get_status_name(int status);


#endif	// __COMMON_H
