#include "txcommon.h"


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


	// params set
	Params_type_t params = paramsget("../lib/params.dat");

	printf("Verifier %d START!\n", thrid);

	sprintf(tmp, "VER%02d.ver", thrid);	// out file
	outfp = fopen(tmp, "w+b");
	assert(outfp != NULL);

	sprintf(endmark, "%s CLOSE", filter);

	sleepms(10);

	while (1)
	{
		txdata_t txdata;
		tx_send_token_t txsend;
		xserial txsz(4 * 1024);
		string	data;

		count++;
		txdata = _verifyq.pop();

		txsz.setstring(txdata.data);
		printf("\n");
		printf("-----VERIFY:\n");
		deseriz(txsz, txsend, 0);
		deseriz(txsz, txdata.sign, 0);

		txdata.valid = verify_message_bin(txsend.from_addr.c_str(), txdata.sign.signature.c_str(), 
					txdata.data.c_str(), txdata.sign.data_length, &params.AddrHelper);

		// 검증 요청하는 경우는, 검증 결과만 전송함 (txid or sign + 결과(OK/FAIL))
		if (txdata.status == STAT_VERIFY_REQUEST)
		{
			send_verify_result(txdata);
		}
		// Verification success
		else if (txdata.valid)
		{
			if (txdata.status == STAT_INIT)
			{
				txdata.status = STAT_VERIFY_REQUEST;
				_sendq.push(txdata);	// broadcast to other nodes... (request verification)

				_mempoolq.push(txdata);	// put mempool
			}
		}
		// Verification failed
		else
		{
			if (txdata.status == STAT_INIT)
			{
				send_verify_result(txdata);
			}
		}

		fprintf(outfp, "VER%02d: %7d: %s signature=%s\n", thrid, count, 
			txdata.valid == 1 ? "true" : "false", txdata.sign.signature.c_str());
		fflush(outfp);

#ifdef DEBUG
#else
		if (count % 10000 == 0)
#endif
			printf("VER%02d: Veri %7d veriq=%5ld\n", thrid, count, _mempoolq.size());
	}

	fclose(outfp);

	tmend = xgetclock();
	printf("VERI: Veri time=%.3f / %.1f/sec\n",
		tmend - tmstart, count / (tmend - tmstart));

	fprintf(stderr, "\nVerifier %d END!\n", thrid);

	pthread_exit(NULL);

	return NULL;
}


//
// 검증 결과만 발송: txid or sign + 검증 결과(type)
//
void	send_verify_result(txdata_t& txdata)
{
	tx_verify_reply_t txreply;

	txreply.filter = ZMQ_FILTER;
	txreply.type = txdata.valid ? STAT_VERIFY_OK : STAT_VERIFY_FAIL;
	txreply.signature = txdata.sign.signature;
	txreply.txid = txdata.txid;

	xserial txsz(1 * 1024);
	seriz_add(txsz, txreply);

	txdata.data = txsz.getdata();
	txdata.status = txdata.valid ? STAT_VERIFY_OK : STAT_VERIFY_FAIL;

	_sendq.push(txdata);	// broadcast to other nodes... (request verification)
}
