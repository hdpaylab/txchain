#include "txcommon.h"


int	tx_verify(txdata_t& txdata);


//
// 각 노드 사이에서 전파되는 정보를 수신하여 처리
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

	// 디버깅 파일 생성 
	strcpy(tmp, peer);
	strcat(tmp, ".out");
	tp = strchr(tmp, ':');
	assert(tp != NULL);
	*tp = '_';
	outfp = fopen(tmp, "w+b");
	assert(outfp != NULL);

	// ZMQ PUB-SUB 구조 초기화
	zmq::context_t context_sub(1);
	zmq::socket_t xsock(context_sub, ZMQ_SUB);

	// ZMQ setup connection
	sprintf(peerstr, "tcp://%s", peer);
	xsock.connect(peerstr);
	
	// ZMQ setup filter
	xsock.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));

	// ZMQ Buffer & queue setup
	int bufsize = 1 * 1024 * 1024;	// 1MB buffer
	xsock.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	int qsize = 10000;		// 10000 
	xsock.setsockopt(ZMQ_RCVHWM, &qsize, sizeof(qsize));

	sleepms(100);

	while (1)
	{
		txdata_t txdata;

		//  각 노드에서 오는 패킷을 순서대로 읽음
		txdata.orgdataser = s_recv(xsock);
		count++;

		printf("\n\n=====Subscriber for broadcast: (recv from ZMQ::PUB-SUB)\n");

		// 필터 제거 
		const char *filter = txdata.orgdataser.c_str();

		string realdata(&filter[strlen(ZMQ_FILTER)], txdata.orgdataser.length() - strlen(ZMQ_FILTER));
		txdata.orgdataser = realdata;

		// 헤더와 바디 구분 파싱 
		tx_header_t *hp = parse_header_body(txdata);

		printf("    %s data length=%ld (%s) real length=%ld\n", 
			get_type_name(hp->type), txdata.orgdataser.size(), ZMQ_FILTER, txdata.orgdataser.size());

		// TX 유형별 처리 
		tx_verify(txdata);

		// 디버깅 위해 파일에 저장 
		fprintf(outfp, "%7d: len=%ld\n", count, hp->data_length);
		fflush(outfp);
#ifdef DEBUG
#else
		if (count % 100000 == 0)
			printf("    Processed %d  recvq=%5ld\n", count, _verifyq.size());
#endif
	}

	xsock.close();

	fclose(outfp);

	pthread_exit(NULL);

	return NULL;
}


//
// Client 요청을 수신: Client 요청 수신 후 즉시 응답 
//
void	*thread_client(void *info_p)
{
        int	clientport = *(int *)info_p;
	int	count = 0;
	char	tmp[256] = {0}, ip_port[100] = {0};


	printf("CLIENT: client port=%d START!\n", clientport);

	// 디버깅 파일 생성 
	snprintf(tmp, sizeof(tmp), "CLIENT %d.out", clientport);
	FILE *outfp = fopen(tmp, "w+b");	// output file
	assert(outfp != NULL);

	// ZMQ setup: REQ-REP model 
	zmq::context_t context(1);

	zmq::socket_t responder(context, ZMQ_REP);
	sprintf(ip_port, "tcp://*:%d", clientport);
	responder.bind(ip_port);

	// ZMQ Buffer & queue setup
	int bufsize = 64 * 1024;	// 64k buffer
	responder.setsockopt(ZMQ_SNDBUF, &bufsize, sizeof(bufsize));

	bufsize = 64 * 1024;		// 64k buffer
	responder.setsockopt(ZMQ_RCVBUF, &bufsize, sizeof(bufsize));

	while(1)
	{
		txdata_t txdata;

		//  Client 요청 수신 
		txdata.orgdataser = s_recv(responder);
		count++;

		// 헤더와 바디 분리 파싱 
		tx_header_t *hp = parse_header_body(txdata);

		printf("\n\n=====Subscriber for client: (recv from ZMQ::REQ-REPLY)\n");

		// 요청 유형에 따라서 처리: 전체 노드로 broadcast 필요한 경우 1이 리턴됨 
		int broadcast_tx = tx_verify(txdata);

		// 정상적으로 검증되고 전체 노드로 broadcast 필요한 tx인 경우 (verify는 중복 처리 안함)
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
// Return: 전체 노드로 broadcast 필요한 경우에 1 리턴
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
	// 다른 노드에서 블록 생성하겠다고 요청이 온 경우
	// 일정 시간 동안 동일 블록 생성 요청을 못하게 block됨 (timeout)
	else if (hp->type == TX_BLOCK_GEN_REQ)
	{
		ps_block_gen_req(txdata);

		return 0;
	}
	// 블록 생성과 관련된 응답을 받음 
	// 각 서버별로 응답을 모아서, 일정 비율 이상이면 블록 발행 
	else if (hp->type == TX_BLOCK_GEN_REPLY)
	{
		ps_block_gen_reply(txdata);

		return 0;
	}
	// 실제 블록 생성 명령 
	else if (hp->type == TX_BLOCK_GEN)
	{
		ps_block_gen(txdata);

		return 0;
	}
	else if (hp->type == TX_CREATE_TOKEN)
	{
		tx_create_token_t create_token;

		deseriz(bodyszr, create_token, 0);
		from_addr = create_token.from_addr;

		hp->valid = verify_message_bin(from_addr.c_str(), hp->signature.c_str(), 
					txdata.bodyser.c_str(), hp->data_length, &_netparams.AddrHelper);
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
					txdata.bodyser.c_str(), hp->data_length, &_netparams.AddrHelper);
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
		printf("ERROR: Subscriber: No handling routine for TX type=%s\n",
			get_type_name(hp->type));

		return 0;
	}

	return 0;
}