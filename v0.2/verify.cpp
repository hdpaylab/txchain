#include "txcommon.h"


void	*thread_verifier(void *info_p)
{
	int	thrid = *(int *)info_p;
	int	count = 0;
	double	tmstart, tmend;

	FILE	*outfp = NULL;
	char	ESC = TX_DELIM;
	char	endmark[100] = {0};
	char	tmp[4096] = {0}, *buf = NULL;
	const char *filter = ZMQ_FILTER;	// s_sendmore()로 publisher에서 보내는 것만 수용함 


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
		string data, pubkey, message, signature;

		count++;
		txdata = _recvq.pop();
		data = txdata.data;
		txdata.verified = 0;

		buf = (char *)data.c_str();
	//	printf("VER%02d: data=%s seq=%d status=%X\n", thrid, buf, txdata.seq, txdata.status);

		if (strlen(buf) >= strlen(filter) + 8)	// 4 byte filter, 8 byte number
		{
			//                 pp       mp        sp
			// "!@#$####### ESCpubkeyESCmessageESCsignatureESC"
			char	*pp = strchr(buf, ESC); 
			if (pp)
			{
				*pp = 0; pp++;
				char *mp = strchr(pp, ESC);
				if (mp)
				{
					*mp = 0; mp++;
					char *sp = strchr(mp, ESC);
					if (sp) {
						*sp = 0; sp++;
						pubkey = pp;
						message = mp;
						signature = sp;
					}
				}
			}
		}

		if (pubkey.length() == 0 || message.length() == 0 || signature.length() == 0)
		{
			fprintf(stderr, "ERROR: Verifier %d: pubkey len=%ld message len=%ld signature len=%ld\n",
				thrid, pubkey.length(), message.length(), signature.length());
			continue;
		}

		txdata.verified = verify_message(pubkey.c_str(), signature.c_str(), 
					message.c_str(), &params.AddrHelper);

	//	printf("	pubkey	: [%s]\n", pubkey.c_str());
	//	printf("	message	: [%s]\n", message.c_str());
	//	printf("	signature	: [%s]\n", signature.c_str());
	//	printf("	VERITY	: %d\n", txdata.verified);

		fprintf(outfp, "VER%02d: %7d: %s signature=%s\n",
			thrid, count, txdata.verified == 1 ? "true" : "false", signature.c_str());
		fflush(outfp);

		_veriq.push(txdata);
#ifdef DEBUG
		sleepms(DEBUG_SLEEP_MS);
	//	if (count % 10 == 0)
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

