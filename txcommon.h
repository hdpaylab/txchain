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


//#define TXCHAIN_VERIFY_MODEL_MSGQ	1
#define TXCHAIN_VERIFY_MODEL_VECTOR	2


#define MAX_NODE	100

#define MAX_VERIFIER	16


using namespace std;


typedef struct {
	char	*pubkey;
	char	*message;
	char	*signature;
	int	verified;
}	tx_t;

enum {
	TXCHAIN_STATUS_EMPTY	= 0,
	TXCHAIN_STATUS_READY	= 1,
	TXCHAIN_STATUS_VERIFIED	= 2,
};

typedef struct {
	int	txtype;
	string	data;
	int	verified;
	int	status;		// see above TXCHAIN_STATUS_xxx
}	txdata_t;


////////////////////////////////////////////////////////////////////////////////

Params_type_t paramsget(const string& Path);		// xparams.cpp

void	*thread_publisher(void *info_p);		// xpub.cpp

void	*thread_subscriber(void *info_p);		// xsub.cpp

void	*thread_verifier(void *info_p);			// xverify.cpp

void	*thread_levledb(void *info_p);		// xleveldb.cpp

double	xgetclock();					// util.cpp


extern	int	_nthread;


////////////////////////////////////////////////////////////////////////////////
#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

#define SUBSCRIBER_MSGQ_ID	1234
#define VERIFIER_MSGQ_ID	1235

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ


////////////////////////////////////////////////////////////////////////////////
#ifdef TXCHAIN_VERIFY_MODEL_VECTOR

#define MAX_VECTOR_SIZE		10000

extern	vector<txdata_t> _txv;
extern	int	_push_count, _pop_count;

#endif	// TXCHAIN_VERIFY_MODEL_VECTOR


#endif	// __COMMON_H
