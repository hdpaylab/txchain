#include "txcommon.h"


int	tx_process(txdata_t& txdata);
void	send_verify_result(txdata_t& txdata);


void	*thread_verifier(void *info_p)
{
	int	thrid = *(int *)info_p;
	int	count = 0;
	double	tmstart, tmend;

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

		tx_process(txdata);

		fprintf(outfp, "VER%02d: %7d: %s signature=%s\n", thrid, count, 
			txdata.hdr.valid == 1 ? "true" : "false", txdata.hdr.signature.c_str());
		fflush(outfp);
#ifdef DEBUG
#else
		if (count % 10000 == 0)
#endif
			printf("    Ver%02d: processed %7d veriq=%5ld\n", thrid, count, _mempoolq.size());
	}

	fclose(outfp);

	tmend = xgetclock();
	printf("VERI: Veri time=%.3f / %.1f/sec\n",
		tmend - tmstart, count / (tmend - tmstart));

	fprintf(stderr, "\nVerifier %d END!\n", thrid);

	pthread_exit(NULL);

	return NULL;
}


int	tx_process(txdata_t& txdata)
{
	tx_header_t *hp;
	tx_send_token_t txsend;
	xserial hdrszr, bodyszr;


	hp = &txdata.hdr;
	bodyszr.setstring(txdata.bodyser);

	if (hp->type == TX_CREATE_TOKEN && hp->valid == -1)
	{
		tx_create_token_t create_token;

		deseriz(bodyszr, create_token, 0);
		string from_addr = create_token.from_addr;

		hp->valid = verify_message_bin(from_addr.c_str(), hp->signature.c_str(), 
					txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
		printf("    verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());
	}
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

	// 검증 요청하는 경우는, 검증 결과만 전송함 (txid or sign + 결과(OK/FAIL))
	if (hp->status == STAT_VERIFY_REQUEST)
	{
		send_verify_result(txdata);
	}
	// Verification success
	else if (hp->valid)
	{
		if (hp->status == STAT_INIT)
		{
			hp->nodeid = getpid();
			hp->status = STAT_VERIFY_REQUEST;

			xserial tmpszr;
			seriz_add(tmpszr, txdata.hdr);
			txdata.hdrser = tmpszr.getstring();	// 헤더 serialization 교체 

			printf("    Add to sendq: type=%s status=%s\n",
				get_type_name(hp->type), get_status_name(hp->status));

			_sendq.push(txdata);	// broadcast to other nodes... (request verification)

			printf("    Add to mempoolq: type=%s status=%s\n",
				get_type_name(txdata.hdr.type), get_status_name(txdata.hdr.status));

			_mempoolq.push(txdata);	// put mempool
		}
	}
	// Verification failed
	else
	{
		if (hp->status == STAT_INIT)
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
	xserial hdrszr;

	hdr.nodeid = getpid();
	hdr.type = TX_VERIFY_REPLY;
	hdr.status = 0;
	hdr.data_length = 0;
	hdr.valid = txdata.hdr.valid;
	hdr.signature = txdata.hdr.signature;
	hdr.txid = sha256(txdata.hdr.signature);

	seriz_add(hdrszr, hdr);
	txdata.hdrser = hdrszr.getstring();	// 헤더 serialization 교체 

	printf("    Add to sendq: type=%s status=%s\n",
		get_type_name(hdr.type), get_status_name(hdr.status));

	_sendq.push(txdata);	// broadcast to other nodes... (request verification)
}
