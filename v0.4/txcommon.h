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
#include <stdint.h>
#include <libpq-fe.h>
#include <keys/hs_keys.h>		// KeyPairs
#include <keys/hs_keys_wrapper.h>	// xparams.h
#include <helpers/hs_helpers.h>
#include <keys/keyshelper.h>
#include <keys/key.h>
#include <keys/bitcoinsecret.h>
#include <keys/eccautoinitreleasehandler.h>
#include <keys/bitcoinaddress.h>
#include <structs/hashes.h>		// CSHA256()
#include <utils/utilstrencodings.h>	// HexStr()
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <leveldb/c.h>			// xleveldb.cpp


using namespace std;


#include "lib.h"
#include "tx.h"


#define DEBUG		1		// for debugging
#define DEBUG_SLEEP_MS	1000
#define MAX_TEST_NUM_TX	1000000		// tx count for test..



#define MAX_NODE		100		// 최대 합의용 노드 개수 
#define BLOCK_ITV		1		// 블록 생성 주기 (1초)
#define GENESIS_BLOCK_SIZE	(32 * 1024)	// 32KB


#define MAX_VERIFY_THREADS	4		// number of verifier threads
#define DEFAULT_CLIENT_PORT	6999		// default client management port
#define DEFAULT_CHAIN_PORT	7000		// default chain management port

#define MAX_PUBLISH_QUEUE_SIZE	10000		// publish queue for publisher
#define MAX_VERIFY_QUEUE_SIZE	10000		// verify queue for verifier
#define MAX_MEMPOOL_QUEUE_SIZE	10000		// mempool queue for subscriber
#define MAX_CONSENSUS_QUEUE_SIZE 10000		// consensus queue for subscriber


#define sleepms(n)	usleep(n * 1000)	// ms unit sleep

#define ZMQ_FILTER	"!@#$"			// ZMQ delimiter


typedef unsigned char	uchar;

typedef KeyPairs	keypair_t;


// main.cpp
extern	int	_debug;		// debugging level
extern	int	_nverifier;	// current number of verifier threads
extern	int	_clientport;	// client connection port
extern	keypair_t _keypair;	// keypair of this node

extern	safe_queue<txdata_t>	_sendq;		// send queue for publisher
extern	safe_queue<txdata_t>	_verifyq;	// receive queue for subscriber
extern	safe_queue<txdata_t>	_mempoolq;	// receive queue for verifier
extern	safe_queue<txdata_t>	_leveldbq;	// leveldb queue 
extern	safe_queue<txdata_t>	_consensusq;	// mempool queue (verification reply)

extern	Params_type_t _netparams;		// parameters for sign/verify

void	*thread_publisher(void *info_p);	// pub.cpp
void	*thread_send_test(void *info_p);	// main.cpp
void	*thread_subscriber(void *info_p);	// sub.cpp
void	*thread_client(void *info_p);		// sub.cpp
void	*thread_verifier(void *info_p);		// verify.cpp
void	*thread_txid_info(void *info_p);	// mempool.cpp
void	*thread_levledb(void *info_p);		// leveldb.cpp
void	*thread_consensus(void *info_p);	// consensus.cpp


// ldbio.cpp
extern	leveldb	_systemdb;	// permission, channel 등의 설정 정보 index
extern	leveldb	_txdb;		// block, tx 정보 index
extern	leveldb	_walletdb;	// wallet 정보 index


// common.cpp
const char *get_type_name(int type);
const char *get_status_name(int status);
int		load_params_dat(const char *path = "params.dat");
keypair_t	create_keypair();
keypair_t	create_keypair(const uchar *privkey, size_t keylen);

tx_header_t	*parse_header_body(txdata_t& txdata);
string	dump_tx(const char *title, txdata_t& txdata, bool disp = 1);
int	logprintf(int level, ...);
string	datestring(time_t tm = 0);


// mempool.cpp
extern	map<string, txdata_t> _mempoolmap;	// mempool index (key=txid)
extern	mutex	_mempool_lock;			// mempool lock

int	mempool_add(txdata_t& txdata);
void	mempool_update(string txid, int flag);


// block.cpp
#define TX_TIME_DIFF	0.1			// mempool에 TX 저장한지 최소 0.1초 지나야 다음 블록 포함시킴 

typedef struct {
	int	on_air;				// 현대 블록 생성 명령 작동 중이면 1
	size_t	block_height;			// 블록 번호 
	string	sign_hash;			// block_gen_req로 보낸 signature의 hash 값 
	vector<string> txidlist;		// txid list
}	block_txid_info_t;

extern	block_txid_info_t _self_txid_info;	// 자체적으로 블록 생성을 위한 txidlist
extern	block_txid_info_t _recv_txid_info;	// 블록 생성을 위해서 다른 노드에서 보낸 txidlist

extern	block_info_t	_genesis_block_hdr;	// Genesis block header
extern	block_info_t	_last_block_hdr;	// latest block header
extern	uint32_t	_last_block_file_no;	// 블록 파일 번호 

void	ps_block_gen_req(txdata_t& txdata);
void	ps_block_gen_reply(txdata_t& txdata);
void	ps_block_gen(txdata_t& txdata, block_txid_info_t& txid_info);		// 실제 블록 생성 

int	check_blocks();
int	make_genesis_block(const char *path);
keypair_t load_genesis_block(const char *path);


// consensus.cpp
extern	cssmap	_cssmap;		// consensus.cpp


#endif	// __COMMON_H
