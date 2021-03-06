#include "txcommon.h"


int	tx_verify(txdata_t& txdata);
void	verify_sign(string from_addr, tx_header_t *hp, txdata_t& txdata);


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


	logprintf(2, "SUB : peer=%s START!\n", peer);

	// 디버깅 파일 생성 
	sprintf(tmp, "logs/%s.out", peer);
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

		logprintf(3, "\n\n=====Subscriber for NODE: (ZMQ::PUB-SUB)\n");

		// 필터 제거 
		const char *filter = txdata.orgdataser.c_str();

		string realdata(&filter[strlen(ZMQ_FILTER)], txdata.orgdataser.length() - strlen(ZMQ_FILTER));
		txdata.orgdataser = realdata;

		// 헤더와 바디 구분 파싱 
		parse_header_body(txdata);

		if (txdata.hdr.type != TX_SEND_TOKEN)
			logprintf(2, "%s\n", dump_tx("    Node   : ", txdata, 0).c_str());

		// TX 유형별 처리 
		tx_verify(txdata);

#ifdef DEBUG
		// 디버깅 위해 파일에 저장 
		fprintf(outfp, "%7d: %s \n", count, dump_tx("", txdata, 0).c_str());
		fflush(outfp);
		if (count % 1000 == 0)
			printf("    SUB: recv %d  verifyq=%ld\n", count, _verifyq.size());
#else
		if (count % 100000 == 0)
			printf("    SUB: recv %d  verifyq=%ld\n", count, _verifyq.size());
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


	logprintf(2, "CLIENT: client port=%d START!\n", clientport);

	// 디버깅 파일 생성 
	snprintf(tmp, sizeof(tmp), "logs/client-%d.out", clientport);
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
		parse_header_body(txdata);
		txdata.hdr.recvclock = xgetclock();

		logprintf(3, "\n\n=====Subscriber for CLIENT: (ZMQ::REQ-REPLY)\n");

		logprintf(2, "%s\n", dump_tx("    Client : ", txdata, 0).c_str());

		// 요청 유형에 따라서 처리: 전체 노드로 broadcast 필요한 경우 1이 리턴됨 
		int broadcast_tx = tx_verify(txdata);

		// 정상적으로 검증되고 전체 노드로 broadcast 필요한 tx인 경우 (verify는 중복 처리 안함)
		if (txdata.hdr.valid == 1 && broadcast_tx == 1)
		{
			txdata.hdr.status = STAT_BCAST_TX;

			logprintf(2, "%s\n", dump_tx("    Bcast  : ", txdata, 0).c_str());

			_verifyq.push(txdata);	// send to verify.cpp
		}

#ifdef DEBUG
	//	if (count % 1000 == 0)
			printf("    Client: recv %d: %s \n", count, get_type_name(txdata.hdr.type));
#else
		if (count % 10000 == 0)
			printf("    Client: recv %d: %s \n", count, get_type_name(txdata.hdr.type));
#endif

		// Send reply back to client
		s_send(responder, txdata.hdr.txid);
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
	string	from_addr;

	hp = &txdata.hdr;

	// 다른 노드에서 TX 전파한 경우: 검증 후 mempool에 넣음 
	if (hp->status == STAT_BCAST_TX)
	{
		hp->valid = -1;
		hp->status = STAT_ADD_TO_MEMPOOL;

		_verifyq.push(txdata);			// send to verify.cpp
	}
	// 다른 노드에서 블록 생성하겠다고 요청이 온 경우
	// 일정 시간 동안 동일 블록 생성 요청을 못하게 block됨 (timeout)
	else if (hp->type == TX_BLOCK_GEN_REQ)
	{
		ps_block_gen_req(txdata);
	}
	// 블록 생성과 관련된 응답을 받음 
	// 각 서버별로 응답을 모아서, 일정 비율 이상이면 블록 발행 
	else if (hp->type == TX_BLOCK_GEN_REPLY)
	{
		ps_block_gen_reply(txdata);
	}
	// 실제 블록 생성 명령 
	else if (hp->type == TX_BLOCK_GEN)
	{
		ps_block_gen(txdata, _recv_txid_info);
	}
	else if (hp->type == TX_VERIFY_REPLY)
	{
		tx_verify_reply_t verify_reply;

		strdeseriz(txdata.bodyser, verify_reply, 1);
	}
	else if (hp->type == TX_CREATE_TOKEN)
	{
		tx_create_token_t create_token;

		strdeseriz(txdata.bodyser, create_token, 0);
		from_addr = create_token.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    CREATE_TOKEN: ", txdata, 0).c_str());

		if (cmd_create_token(txdata, create_token) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else if (hp->type == TX_SEND_TOKEN)
	{
		tx_send_token_t send_token;

		strdeseriz(txdata.bodyser, send_token, 0);
		from_addr = send_token.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    SEND_TOKEN: ", txdata, 0).c_str());

		if (cmd_send_token(txdata, send_token) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else if (hp->type == TX_CREATE_CHANNEL)
	{
		tx_create_channel_t create_channel;

		strdeseriz(txdata.bodyser, create_channel, 0);
		from_addr = create_channel.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    CREATE_CHANNEL: ", txdata, 0).c_str());

		if (cmd_create_channel(txdata, create_channel) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else if (hp->type == TX_PUBLISH_CHANNEL)
	{
		tx_publish_channel_t publish_channel;

		strdeseriz(txdata.bodyser, publish_channel, 0);
		from_addr = publish_channel.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    PUBLISH_CHANNEL: ", txdata, 0).c_str());

		if (cmd_publish_channel(txdata, publish_channel) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else if (hp->type == TX_CREATE_CONTRACT)
	{
		tx_create_contract_t create_contract;

		strdeseriz(txdata.bodyser, create_contract, 0);
		from_addr = create_contract.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    CREATE_CONTRACT: ", txdata, 0).c_str());

		if (cmd_create_contract(txdata, create_contract) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else if (hp->type == TX_DESTROY)
	{
		tx_destroy_t destroy;

		strdeseriz(txdata.bodyser, destroy, 0);
		from_addr = destroy.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    DESTROY: ", txdata, 0).c_str());

		if (cmd_destroy(txdata, destroy) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else if (hp->type == TX_GRANT || hp->type == TX_REVOKE)
	{
		tx_grant_t grant;

		strdeseriz(txdata.bodyser, grant, 0);
		from_addr = grant.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    GRANT: ", txdata, 0).c_str());

		if (cmd_grant_revoke(txdata, grant) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else if (hp->type == TX_CREATE_WALLET)
	{
		tx_create_wallet_t create_wallet;

		strdeseriz(txdata.bodyser, create_wallet, 0);
		from_addr = create_wallet.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    CREATE_WALLET: ", txdata, 0).c_str());

		if (cmd_create_wallet(txdata, create_wallet) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else if (hp->type == TX_CREATE_ACCOUNT)
	{
		tx_create_account_t create_account;

		strdeseriz(txdata.bodyser, create_account, 0);
		from_addr = create_account.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    CREATE_ACCOUNT: ", txdata, 0).c_str());

		if (cmd_create_account(txdata, create_account) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else if (hp->type == TX_CONTROL)
	{
		tx_control_t control;

		strdeseriz(txdata.bodyser, control, 0);
		from_addr = control.from_addr;

		verify_sign(from_addr, hp, txdata);

		logprintf(3, "%s\n", dump_tx("    CONTROL: ", txdata, 0).c_str());

		if (cmd_control(txdata, control) == false)
			return 0;		// ERROR: DO NOT BROADCAST
		return 1;
	}
	else
	{
		fprintf(stderr, "ERROR: Subscriber: No handling routine for TX type=%s\n",
			get_type_name(hp->type));
	}

	return 0;	// DO NOT BROADCAST
}


void	verify_sign(string from_addr, tx_header_t *hp, txdata_t& txdata)
{
	hp->valid = verify_message_bin(from_addr.c_str(), hp->signature.c_str(), 
			txdata.bodyser.c_str(), hp->data_length, &_netparams.AddrHelper);

	hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
}
