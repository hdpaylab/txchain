#include "txcommon.h"


void	*thread_verify_msgq(void *info_p)
{
	int	thrid = *(int *)info_p;
	int	count = 0;
	int	rmsqid = -1;
	int	smsqid = -1;

	// params set
	Params_type_t params = paramsget("params.dat");

	FILE	*outfp = NULL;
	char	ESC = '|';
	tx_t	tx;
	char	endmark[100] = {0};
	char	tmp[4096] = {0}, *buf = NULL;
	const char *filter = "!@#$";	// s_sendmore()로 publisher에서 보내는 것만 수용함 


	printf("Verifier %d START!\n\n", thrid);

	// message queue connect
	rmsqid = msgget( (key_t)1234, IPC_CREAT | 0666);
	if (rmsqid == -1) {
		perror("msgget(1234)");
	}

	// message queue connect
	smsqid = msgget( (key_t)1235, IPC_CREAT | 0666);
	if (smsqid == -1) {
		perror("msgget(1235)");
	}

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
		data_t msq_data;

		// message queue recv
		if (msgrcv(rmsqid, &msq_data, BUFF_SIZE, 1, 0) == -1) {
			fprintf(stderr, "ERROR: Verifier %d: message queue recv error - %d", 
				thrid, count);
		}

		std::string data(msq_data.mtext);
//continue;	// msg 수신만 하는 경우 

		count++;
		if (count % 10000 == 0)
		//	printf("Verifier %d: count=%d data=%s\n", thrid, count, data.c_str());
			printf("Verifier %d: count=%d \n", thrid, count);

		buf = strdup(data.c_str());
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
			free(buf);
			continue;
		}

	//	printf("xv:pubkey	: [%s]\n", tx.pubkey);
	//	printf("xv:message	: [%s]\n", tx.message);
	//	printf("xv:signature	: [%s]\n", tx.signature);

		tx.verified = verify_message(tx.pubkey, tx.signature, tx.message, &params.AddrHelper);
	//	printf("xv:VERITY	: %d\n", tx.verified);

		fprintf(outfp, "Verifier %d: %8d: %s signature=%s\n",
			thrid, count, tx.verified == 1 ? "true" : "false", tx.signature);

		// message queue send
		if (msgsnd(smsqid,
			&msq_data, strlen(msq_data.mtext), 0) == -1) {
			fprintf(stderr, "ERROR: Verifier %d: message queue send error - %d: %s\n",
				thrid, count, data.c_str());
		}
		fflush(outfp);

		free(buf);
	}

	fclose(outfp);

	fprintf(stderr, "Verifier %d:	%8d    \n", thrid, count);

	fprintf(stderr, "\nVerifier %d END!\n", thrid);

	pthread_exit(NULL);

	return 0;
}

