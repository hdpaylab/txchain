#include "txcommon.h"


void	*thread_levledb(void *info_p)
{
	leveldb_t *ldb = NULL;
	leveldb_options_t *options = NULL;
	leveldb_writeoptions_t *woptions = NULL;
	char	*err = NULL;
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

	// OPEN 
	options = leveldb_options_create();
	leveldb_options_set_create_if_missing(options, 1);
	ldb = leveldb_open(options, "testdb", &err);

	if (err != NULL) {
		fprintf(stderr, "ERROR: Level DB open failed!\n");
		return 0;
	}

	// reset error var 
	leveldb_free(err); err = NULL;

	tmstart = xgetclock();

	while (1)
	{
		txdata_t txdata;

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

		// WRITE 
		woptions = leveldb_writeoptions_create();
		leveldb_put(ldb, woptions, "key", 3, txdata.data.c_str(), txdata.data.length(), &err);

		if (err != NULL)
		{
			fprintf(stderr, "ERROR: Level DB write failed!\n");
			sleepms(1);
			continue;
		}

		leveldb_free(err); err = NULL;

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

	// CLOSE 
	leveldb_close(ldb);

	// DESTROY 
	leveldb_destroy_db(options, "testdb", &err);

	if (err != NULL) {
		fprintf(stderr, "Destroy fail.\n");
		return 0;
	}

	leveldb_free(err); err = NULL;

	return 0;
}
