#include "txcommon.h"
#include "leveldb.h"


void	*thread_levledb(void *info_p)
{
	int	sendport = *(int *)info_p;
	int	count = 0;
	char	tmp[200];
	double	tmstart, tmend;

#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

	data_t	msq_data;

	// message queue connect
	int rmsqid = msgget( (key_t)VERIFIER_MSGQ_ID, IPC_CREAT | 0666);
	if (rmsqid == -1) {
		perror("msgget(VERIFIER_MSGQ_ID)");
		exit(-1);
	}

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ

	sprintf(tmp, "test-%d.db", sendport);
	leveldb db(tmp);

	tmstart = xgetclock();

	while (1)
	{
		string data;

#ifdef TXCHAIN_VERIFY_MODEL_QUEUE

		txdata_t txdata;
		char	key[16] = {0};

		txdata = _veriq.pop();
		data = txdata.data;

#endif // TXCHAIN_VERIFY_MODEL_QUEUE

#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

		// message queue recv
		if (msgrcv(rmsqid, &msq_data, BUFF_SIZE, 1, 0) == -1) {
			fprintf(stderr, "ERROR: message queue recv error\n");
			sleepms(1);
			continue;
		}

		data = msq_data.mtext;

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ

		sprintf(key, "key %d", count);

		db.put(key, txdata.data);

		count++;
		if (count % 100000 == 0)
		{
			printf("Level DB: %d\n", count);
		}
	}

	tmend = xgetclock();
	printf("LDB : Recv time=%.3f / %.1f/sec\n",
		tmend - tmstart, count / (tmend - tmstart));

//	db.close();

	return 0;
}
