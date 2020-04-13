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
#include "xmsq.h"


#define MAX_NODE	100

#define MAX_VERIFY	4


using namespace std;


typedef struct {
	char	*pubkey;
	char	*message;
	char	*signature;
	int	verified;
}	tx_t;


Params_type_t paramsget(const string& Path);	// xparams.cpp

void	*thread_publisher(void *info_p);	// xpub.cpp

void	*thread_subscriber(void *info_p);	// xsub.cpp

void	*thread_verify(void *info_p);		// xverify.cpp

void	*thread_levledb(void *info_p);	// xleveldb.cpp

double	xgetclock();				// util.cpp


#endif	// __COMMON_H
