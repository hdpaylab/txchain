#include "txcommon.h"
#include "leveldb.h"


void	*thread_levledb(void *info_p)
{
	int	count = 0;
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

	leveldb db("test.db");

	tmstart = xgetclock();

	while (1)
	{
		txdata_t txdata;
		char	key[16] = {0};

		count++;
		txdata = _veriq.pop();

#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

		// message queue recv
		if (msgrcv(rmsqid, &msq_data, BUFF_SIZE, 1, 0) == -1)
		{
			fprintf(stderr, "ERROR: message queue recv error\n");
			sleepms(1);
			continue;
		}

		data = msq_data.mtext;

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ

		sprintf(key, "key %d", count);

		db.put(key, txdata.data);

#ifdef DEBUG
		sleepms(DEBUG_SLEEP);
	//	if (count % 10 == 0)
#else
		if (count % 100000 == 0)
#endif
			printf("LDB : Recv %7d veriq=%5ld\n", count, _veriq.size());
		if (count >= MAX_TEST_NUM_TX)
			break;
	}

	tmend = xgetclock();
	printf("LDB : Recv time=%.3f / %.1f/sec\n",
		tmend - tmstart, count / (tmend - tmstart));

//	db.close();

	return 0;
}
