#ifndef __COMMON_H
#define __COMMON_H


#include <zmq.hpp>
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <vector>
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
#include <sys/types.h>
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


typedef struct {
	uint32_t	nodeid;
	int		valid;
}	node_valid_t;


// main.cpp
extern	int	_nverifier;	// current number of verifier threads

extern	safe_queue<txdata_t>	_sendq;		// send queue for publisher
extern	safe_queue<txdata_t>	_verifyq;	// receive queue for subscriber
extern	safe_queue<txdata_t>	_mempoolq;	// receive queue for verifier
extern	safe_queue<txdata_t>	_leveldbq;	// leveldb queue 
extern	safe_queue<txdata_t>	_consensusq;	// mempool queue (verification reply)

extern	Params_type_t _netparams;			// parameters for sign/verify

extern	map<string, array<node_valid_t, 100>> _csslist;		// consensus list ("txid":"nodeid-valid")

void	*thread_publisher(void *info_p);	// pub.cpp
void	*thread_send_test(void *info_p);	// main.cpp
void	*thread_subscriber(void *info_p);	// sub.cpp
void	*thread_client(void *info_p);		// sub.cpp
void	*thread_verifier(void *info_p);		// verify.cpp
void	*thread_block_gen(void *info_p);	// mempool.cpp
void	*thread_levledb(void *info_p);		// leveldb.cpp
void	*thread_consensus(void *info_p);	// consensus.cpp


// ldbio.cpp
extern	leveldb	_systemdb;	// 
extern	leveldb	_walletdb;	// 


// common.cpp
const char *get_type_name(int type);
const char *get_status_name(int status);
tx_header_t	*parse_header_body(txdata_t& txdata);
void	mempool_update(string txid, int flag);

// mempool.cpp
extern	vector<txdata_t> _mempool;		// mempool
extern	size_t _mempool_count;			// number of mempool
extern	map<string, txdata_t *>	_mempoolmap;	// mempool index (key=txid)
extern	mutex	_mempool_lock;			// mempool lock

int	mempool_add(txdata_t& txdata);

// block.cpp
void	ps_block_gen_req(txdata_t& txdata);
void	ps_block_gen_reply(txdata_t& txdata);
void	ps_block_gen(txdata_t& txdata);		// 실제 블록 생성 

// consensus.cpp
extern	cssmap	_cssmap;		// consensus.cpp

#endif	// __COMMON_H
