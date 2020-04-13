#include "txcommon.h"


void	*thread_verifier(void *info_p)
{
	int	thrid = *(int *)info_p;
	int	count = 0;

	// params set
	Params_type_t params = paramsget("params.dat");

	FILE	*outfp = NULL;
	char	ESC = '|';
	tx_t	tx;
	char	endmark[100] = {0};
	char	tmp[4096] = {0}, *buf = NULL;
	const char *filter = "!@#$";	// s_sendmore()로 publisher에서 보내는 것만 수용함 


	printf("Verifier %d START!\n\n", thrid);

#ifdef TXCHAIN_VERIFY_MODEL_MSGQ
	// message queue connect
	int rmsqid = msgget( (key_t)SUBSCRIBER_MSGQ_ID, IPC_CREAT | 0666);
	if (rmsqid == -1) {
		perror("msgget(SUBSCRIBER_MSGQ_ID)");
		exit(-1);
	}

	// message queue connect
	int smsqid = msgget( (key_t)VERIFIER_MSGQ_ID, IPC_CREAT | 0666);
	if (smsqid == -1) {
		perror("msgget(VERIFIER_MSGQ_ID)");
		exit(-1);
	}
#endif	// TXCHAIN_VERIFY_MODEL_MSGQ

	sprintf(tmp, "Verifier %d.ver", thrid);
	outfp = fopen(tmp, "w+b");
	assert(outfp != NULL);

	sprintf(endmark, "%s CLOSE", filter);

	sleep (1);

	while (1)
	{
		tx.pubkey = NULL;
		tx.message = NULL;
		tx.signature = NULL;
		tx.verified = -1;

#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

		data_t msq_data;

		// message queue recv
		if (msgrcv(rmsqid, &msq_data, BUFF_SIZE, 1, 0) == -1) {
			fprintf(stderr, "ERROR: Verifier %d: message queue recv error - %d", 
				thrid, count);
		}

		string data(msq_data.mtext);

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ

#ifdef TXCHAIN_VERIFY_MODEL_VECTOR

		int idx = (count * _nthread + thrid) % MAX_VECTOR_SIZE;

		if (_txv[idx].status != TXCHAIN_STATUS_READY)
		{
			usleep(10);
			continue;
		}

		string data = _txv[idx].data;

#endif	// TXCHAIN_VERIFY_MODEL_VECTOR

//continue;	// msg 수신만 하는 경우 

		count++;
		if (count % 10000 == 0)
		//	printf("Verifier %d: count=%d data=%s\n", thrid, count, data.c_str());
			printf("Verifier %d: count=%d \n", thrid, count);

		buf = (char *)data.c_str();
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
						tx.pubkey = pp;
						tx.message = mp;
						tx.signature = sp;
					}
				}
			}
		}

		if (tx.pubkey == NULL || tx.message == NULL || tx.signature == NULL)
		{
			fprintf(stderr, "ERROR: Verifier %d: pubkey len=%d message len=%d signature len=%d\n",
				thrid, tx.pubkey ? (int)strlen(tx.pubkey) : 0, 
				tx.message ? (int)strlen(tx.message) : 0,
				tx.signature ? (int)strlen(tx.signature) : 0);
			continue;
		}

	//	printf("xv:pubkey	: [%s]\n", tx.pubkey);
	//	printf("xv:message	: [%s]\n", tx.message);
	//	printf("xv:signature	: [%s]\n", tx.signature);

		tx.verified = verify_message(tx.pubkey, tx.signature, tx.message, &params.AddrHelper);
	//	printf("xv:VERITY	: %d\n", tx.verified);

		fprintf(outfp, "Verifier %d: %8d: %s signature=%s\n",
			thrid, count, tx.verified == 1 ? "true" : "false", tx.signature);

#ifdef TXCHAIN_VERIFY_MODEL_VECTOR

		_txv[idx].verified = tx.verified;
		_txv[idx].status = TXCHAIN_STATUS_VERIFIED;	// 상태 업데이트

#endif	// TXCHAIN_VERIFY_MODEL_VECTOR

#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

		// message queue send
		if (msgsnd(smsqid,
			&msq_data, strlen(msq_data.mtext), 0) == -1) {
			fprintf(stderr, "ERROR: Verifier %d: message queue send error - %d: %s\n",
				thrid, count, data.c_str());
		}

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ

		fflush(outfp);
	}

	fclose(outfp);

	fprintf(stderr, "Verifier %d:	%8d    \n", thrid, count);

	fprintf(stderr, "\nVerifier %d END!\n", thrid);

	pthread_exit(NULL);

	return 0;
}

