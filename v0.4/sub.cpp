#include "txcommon.h"


int	tx_verify(txdata_t& txdata);
void	ps_block_sync(txdata_t& txdata);


//
// thread for VECTOR model
//
void	*thread_subscriber(void *info_p)
{
	char	*peer = (char *)info_p;
	int	count = 0;

	FILE	*outfp = NULL;
	char	peerstr[100] = {0};
	char	tmp[4096] = {0}, *tp = NULL;
	const char *filter = ZMQ_FILTER;


	printf("SUB : peer=%s START!\n", peer);

	zmq::context_t context_sub(1);
	zmq::socket_t xsock(context_sub, ZMQ_SUB);

	strcpy(tmp, peer);
	strcat(tmp, ".out");
	tp = strchr(tmp, ':');
	assert(tp != NULL);
	*tp = '_';
	outfp = fopen(tmp, "w+b");
	assert(outfp != NULL);

	// ZMQ setup 
	sprintf(peerstr, "tcp://%s", peer);
	xsock.connect(peerstr);
	
	xsock.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

	int bufsize = 1 * 1024 * 1024;	// 1MB buffer
	xsock.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	int qsize = 10000;		// 10000 
	xsock.setsockopt(ZMQ_RCVHWM, &qsize, sizeof(qsize));

	sleepms(100);

	while (1)
	{
		txdata_t txdata;

		//  Wait for next request from publisher
		txdata.orgdataser = s_recv(xsock);

		printf("\n\n=====Subscriber for broadcast: (recv from ZMQ::PUB-SUB)\n");

		// 필터 제거 
		const char *filter = txdata.orgdataser.c_str();

		string realdata(&filter[strlen(ZMQ_FILTER)], txdata.orgdataser.length() - strlen(ZMQ_FILTER));
		txdata.orgdataser = realdata;

		count++;
		tx_header_t *hp = parse_header_body(txdata);

		printf("    %s data length=%ld (%s) real length=%ld\n", 
			get_type_name(hp->type), txdata.orgdataser.size(), ZMQ_FILTER, txdata.orgdataser.size());

		tx_verify(txdata);

		fprintf(outfp, "%7d: len=%ld\n", count, hp->data_length);
		fflush(outfp);
#ifdef DEBUG
#else
		if (count % 100000 == 0)
			printf("    Processed %d  recvq=%5ld\n", count, _verifyq.size());
#endif
	}

	fclose(outfp);

	xsock.close();

	pthread_exit(NULL);

	return NULL;
}


//
// Client thread (Client request management)
//
// 1. Verify tx and returns the result immediately.
// 2. Push into verifyq if valid tx. (status=STAT_INIT)
//
void	*thread_client(void *info_p)
{
        int	clientport = *(int *)info_p;
	char	tmp[256] = {0}, ip_port[100] = {0};


	printf("CLIENT: client port=%d START!\n", clientport);

	snprintf(tmp, sizeof(tmp), "CLIENT %d.out", clientport);
	FILE *outfp = fopen(tmp, "w+b");	// output file
	assert(outfp != NULL);

	zmq::context_t context(1);

	zmq::socket_t responder(context, ZMQ_REP);
	sprintf(ip_port, "tcp://*:%d", clientport);
	responder.bind(ip_port);

	int bufsize = 64 * 1024;	// 64k buffer
	responder.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

	bufsize = 64 * 1024;		// 64k buffer
	responder.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	int	count = 0;

	while(1)
	{
		txdata_t txdata;

		//  Wait for next request from client
		txdata.orgdataser = s_recv(responder);
		count++;

		tx_header_t *hp = parse_header_body(txdata);

		printf("\n\n=====Subscriber for client: (recv from ZMQ::REQ-REPLY)\n");

		int broadcast_tx = tx_verify(txdata);

		// 정상적으로 검증된 tx인 경우에만 verify 수행함 
		if (hp->valid == 1 && broadcast_tx)
		{
			hp->status = STAT_BCAST_TX;
			printf("    Add to verifyq: type=%s status=%s\n",
				get_type_name(hp->type), get_status_name(hp->status));

			_verifyq.push(txdata);	// send to verify.cpp
		}

#ifdef DEBUG
#else
		if (count % 10000 == 0)
#endif
			printf("    Client request: count=%d sign=%s\n", count, hp->signature.c_str());

		// Send reply back to client
		s_send(responder, hp->txid);
	}

	fclose(outfp);

	pthread_exit(NULL);

	return NULL;
}


//
// returns 1 if verify required
//
int	tx_verify(txdata_t& txdata)
{
	tx_header_t *hp;
	xserialize bodyszr;
	string	from_addr;

	hp = &txdata.hdr;
	bodyszr.setstring(txdata.bodyser);

	// 다른 노드에서 TX 전파한 경우: 검증 후 mempool에 넣음 
	if (hp->status == STAT_BCAST_TX)
	{
		hp->valid = -1;
		hp->status = STAT_ADD_TO_MEMPOOL;

		printf("    Add to verifyq: type=%s status=%s\n",
			get_type_name(hp->type), get_status_name(hp->status));

		_verifyq.push(txdata);			// send to verify.cpp
	}
	else if (hp->type == TX_BLOCK_SYNC_REQ)
	{
		ps_block_sync(txdata);

		return 0;
	}
	else if (hp->type == TX_BLOCK_SYNC_REPLY)
	{
		printf("    TX_BLOCK_SYNC_REPLY: fail=%d\n", hp->status);
		printf("    각 서버별로 오는 응답 받아서 BLOCK 적용 broadcast\n");

		hp->valid = verify_message_bin(hp->from_addr.c_str(), hp->signature.c_str(), 
					txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
		printf("    TX_BLOCK_SYNC_REPLY verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());

		return 0;
	}
	else if (hp->type == TX_CREATE_TOKEN)
	{
		tx_create_token_t create_token;

		deseriz(bodyszr, create_token, 0);
		from_addr = create_token.from_addr;

		hp->valid = verify_message_bin(from_addr.c_str(), hp->signature.c_str(), 
					txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
		printf("    CREATE_TOKEN verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());

		return 1;
	}
	else if (hp->type == TX_SEND_TOKEN)
	{
		tx_send_token_t send_token;

		deseriz(bodyszr, send_token, 0);
		from_addr = send_token.from_addr;

		hp->valid = verify_message_bin(from_addr.c_str(), hp->signature.c_str(), 
					txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
		printf("    SEND_TOKEN verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());

		return 1;
	}
	else if (hp->type == TX_VERIFY_REPLY)
	{
		tx_verify_reply_t verify_reply;

		deseriz(bodyszr, verify_reply, 1);

		return 0;
	}
	else
	{
		printf("ERROR: Unknown TX type=%08X\n", hp->type);

		return 0;
	}

	return 0;
}


static	const char *privkey = "LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm";	// privkey
static	const char *from_addr = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";	
static	const char *to_addr = "HUGUrwcFy1VC91nq7tRuZpaJqndoHDw64e";


//
// BLOCK_SYNC_REQ 요청 처리: 해당 TX 검증 => 각 TXID 존재여부 확인 => 이상 없으면 OK 리턴 
//
void	ps_block_sync(txdata_t& txdata)
{
	tx_header_t *hp;
	xserialize hdrszr, bodyszr, newbodyszr;

	hp = &txdata.hdr;
	bodyszr.setstring(txdata.bodyser);

	hp->valid = verify_message_bin(hp->from_addr.c_str(), hp->signature.c_str(), 
				txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
	hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
	printf("    BLOCK_SYNC verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());

	if (hp->valid == 0)
		return;

	int	ntotal = 0, nfail = 0;

	while (1)
	{
		tx_block_txid_t block_sync;

		int ret = deseriz(bodyszr, block_sync, 0);
		if (ret == 0)
			break;
		ntotal++;

		txdata_t *txp = _mempoolmap[block_sync.txid];
		if (txp == NULL)
		{
			printf("TXID NOT FOUND: %s\n", block_sync.txid.c_str());
			nfail++;
		}
		else
		{
			txdata_t& txdata = *_mempoolmap[block_sync.txid];
			txdata.hdr.flag |= FLAG_NEXT_BLOCK;
			printf("Marked as next block TXID=%s\n", block_sync.txid.c_str());
		}
	}

	txdata_t newtxdata;
	tx_header_t hdr;
	tx_block_sync_reply_t reply;

	hdr.nodeid = getpid();
	hdr.type = TX_BLOCK_SYNC_REPLY;
	hdr.status = -nfail;
	hdr.data_length = 0;
	hdr.valid = -1;
	hdr.txid = "";
	hdr.from_addr = from_addr;
	hdr.txclock = xgetclock();
	hdr.flag = 0;

	reply.ntotal = ntotal;
	reply.nfail = nfail;

	seriz_add(newbodyszr, reply);
	hdr.data_length = newbodyszr.size();

	hdr.signature = sign_message_bin(privkey, newbodyszr.data(), newbodyszr.size(), 
				&_params.PrivHelper, &_params.AddrHelper);
	seriz_add(hdrszr, hdr);

	newtxdata.hdrser = hdrszr.getstring();	// 헤더 serialization 교체 
	newtxdata.bodyser = newbodyszr.getstring();
	newtxdata.orgdataser = string();

	printf("    Add to sendq(TX_BLOCK_SYNC_REPLY): type=%s nfail=%d\n",
		get_type_name(hdr.type), hdr.status);

	_sendq.push(newtxdata);	// broadcast to other nodes... (request verification)
}

