#ifndef __COMMON_H
#define __COMMON_H


#include <zmq.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <keys/hs_keys_wrapper.h>	// xparams.h
#include <sys/time.h>
#include <leveldb/c.h>			// xleveldb.cpp

#include "zhelpers.hpp"


//#define DEBUG_SLOW_MODE			1	// for debugging


//#define TXCHAIN_VERIFY_MODEL_MSGQ	1
#define TXCHAIN_VERIFY_MODEL_VECTOR	2


#define MAX_NODE	100

#define MAX_VERIFIER	4		// thread °³¼ö 


using namespace std;


typedef struct {
	char	*pubkey;
	char	*message;
	char	*signature;
	int	verified;
}	tx_t;

enum {
	TXCHAIN_STATUS_EMPTY	= 0x00000000,
	TXCHAIN_STATUS_READY	= 0x10000000,
	TXCHAIN_STATUS_VERIFIED	= 0x20000000,
};

typedef struct {
	string		data;
	uint32_t	seq;		// TX sequence
	uint32_t	verified;	// 0=fail 1=success -1=none (@TXCHAIN_STATUS_VERIFIED)
					// WARNING: DO NOT MOVE status position!
	uint32_t	status;		// see above TXCHAIN_STATUS_xxx
}	txdata_t;


////////////////////////////////////////////////////////////////////////////////

Params_type_t paramsget(const string& Path);	// xparams.cpp

void	*thread_publisher(void *info_p);	// xpub.cpp

void	*thread_subscriber(void *info_p);	// xsub.cpp

void	*thread_verifier(void *info_p);		// xverify.cpp

void	*thread_levledb(void *info_p);		// xleveldb.cpp

double	xgetclock();				// util.cpp


extern	int	_nthread;	// current number of verifier threads


////////////////////////////////////////////////////////////////////////////////
#ifdef TXCHAIN_VERIFY_MODEL_VECTOR

#define MAX_VECTOR_SIZE		10000

extern	vector<txdata_t> _txv;
extern	int	_push_count, _pop_count;

#endif	// TXCHAIN_VERIFY_MODEL_VECTOR


////////////////////////////////////////////////////////////////////////////////
#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

#define SUBSCRIBER_MSGQ_ID	1234
#define VERIFIER_MSGQ_ID	1235

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ


#endif	// __COMMON_H
