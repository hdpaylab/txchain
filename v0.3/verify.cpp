#include "txcommon.h"


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
		txdata = _recvq.pop();

		txsz.setstring(txdata.data);
		printf("\n");
		printf("VERIFY:\n");
		deseriz(txsz, txsend, 1);
		deseriz(txsz, txdata.sign, 1);

		txdata.verified = verify_message_bin(txsend.from_addr.c_str(), txdata.sign.signature.c_str(), 
					txdata.data.c_str(), txdata.sign.data_length, &params.AddrHelper);

		printf("	VERITY	: %d\n", txdata.verified);

		fprintf(outfp, "VER%02d: %7d: %s signature=%s\n", thrid, count, 
			txdata.verified == 1 ? "true" : "false", txdata.sign.signature.c_str());
		fflush(outfp);

		_veriq.push(txdata);
#ifdef DEBUG
#else
		if (count % 10000 == 0)
#endif
			printf("VER%02d: Veri %7d veriq=%5ld\n", thrid, count, _veriq.size());
	}

	fclose(outfp);

	tmend = xgetclock();
	printf("VERI: Veri time=%.3f / %.1f/sec\n",
		tmend - tmstart, count / (tmend - tmstart));

	fprintf(stderr, "\nVerifier %d END!\n", thrid);

	pthread_exit(NULL);

	return NULL;
}

