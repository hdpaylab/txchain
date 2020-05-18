#include "txcommon.h"


int	verify_process(txdata_t& txdata);
void	send_verify_result(txdata_t& txdata);


void	*thread_verifier(void *info_p)
{
	int	thrid = *(int *)info_p;
	int	count = 0;

	FILE	*outfp = NULL;
	char	endmark[100] = {0};
	char	tmp[256] = {0};
	const char *filter = ZMQ_FILTER;


	printf("Verifier %d START!\n", thrid);

	sprintf(tmp, "VER%02d.ver", thrid);	// out file
	outfp = fopen(tmp, "w+b");
	assert(outfp != NULL);

	sprintf(endmark, "%s CLOSE", filter);

	sleepms(10);

	while (1)
	{
		txdata_t txdata;

		count++;
		txdata = _verifyq.pop();

		printf("\n-----Verifier:\n");

		verify_process(txdata);

		fprintf(outfp, "VER%02d: %7d: %s signature=%s\n", thrid, count, 
			txdata.hdr.valid == 1 ? "true" : "false", txdata.hdr.signature.c_str());
		fflush(outfp);
#ifdef DEBUG
#else
		if (count % 10000 == 0)
#endif
			printf("    Ver%02d: processed %7d veriq=%5ld\n", thrid, count, _verifyq.size());
	}

	fclose(outfp);

	pthread_exit(NULL);

	return NULL;
}


int	verify_process(txdata_t& txdata)
{
	tx_header_t *hp;
	tx_send_token_t txsend;
	xserialize hdrszr, bodyszr;


	hp = &txdata.hdr;
	bodyszr.setstring(txdata.bodyser);

	// TX 전파하는 경우, mempool에 넣기만 함 
	if (hp->status == STAT_ADD_TO_MEMPOOL)
	{
		add_mempool(txdata);	// put mempool
	}
	// client에서 온 TX
	else if (hp->type == TX_CREATE_TOKEN && hp->valid == -1)
	{
		tx_create_token_t create_token;

		deseriz(bodyszr, create_token, 0);
		string from_addr = create_token.from_addr;

		hp->valid = verify_message_bin(from_addr.c_str(), hp->signature.c_str(), 
					txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
		printf("    verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());
	}
	// client에서 온 TX
	else if (hp->type == TX_SEND_TOKEN && hp->valid == -1)
	{
		tx_send_token_t send_token;

		deseriz(bodyszr, send_token, 0);
		string from_addr = send_token.from_addr;

		hp->valid = verify_message_bin(from_addr.c_str(), hp->signature.c_str(), 
					txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
		printf("    verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());
	}
	// Verification success
	else if (hp->valid)
	{
		// 다른 노드로 복사 
		if (hp->status == STAT_BCAST_TX)
		{
			hp->nodeid = getpid();

			xserialize tmpszr;
			seriz_add(tmpszr, txdata.hdr);
			txdata.hdrser = tmpszr.getstring();	// 헤더 serialization 교체 

			printf("    Add to sendq: type=%s status=%s\n",
				get_type_name(hp->type), get_status_name(hp->status));

			add_mempool(txdata);	// put mempool

			printf("    Add to mempoolq: type=%s status=%s size=%ld\n",
				get_type_name(txdata.hdr.type), get_status_name(txdata.hdr.status),
				hp->data_length);

			_sendq.push(txdata);	// broadcast to other nodes... (request verification)
		}
	}
	// Verification failed
	else
	{
		if (hp->status == STAT_BCAST_TX)
		{
			send_verify_result(txdata);
		}
	}

	return 1;
}


//
// 검증 결과만 발송: txid or sign + 검증 결과(type)
//
void	send_verify_result(txdata_t& txdata)
{
	tx_header_t hdr;
	xserialize hdrszr;

	hdr.nodeid = getpid();
	hdr.type = TX_VERIFY_REPLY;
	hdr.status = 0;
	hdr.data_length = 0;
	hdr.valid = txdata.hdr.valid;
	hdr.signature = txdata.hdr.signature;
	hdr.txid = sha256(txdata.hdr.signature);

	seriz_add(hdrszr, hdr);
	txdata.hdrser = hdrszr.getstring();	// 헤더 serialization 교체 

	printf("    Add to sendq(TX_VERIFY_REPLY): type=%s status=%s\n",
		get_type_name(hdr.type), get_status_name(hdr.status));

	_sendq.push(txdata);	// broadcast to other nodes... (request verification)
}
