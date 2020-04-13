#include "txcommon.h"


void	*thread_levledb(void *info_p)
{
	leveldb_t *ldb = NULL;
	leveldb_options_t *options = NULL;
	leveldb_writeoptions_t *woptions = NULL;
	char	*err = NULL;
	int	count = 0;

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

	while (1)
	{

#ifdef TXCHAIN_VERIFY_MODEL_MSGQ

		// message queue recv
		if (msgrcv(rmsqid, &msq_data, BUFF_SIZE, 1, 0) == -1) {
			fprintf(stderr, "ERROR: message queue recv error\n");
			usleep(100);
			continue;
		}

		string data(msq_data.mtext);

#endif	// TXCHAIN_VERIFY_MODEL_MSGQ

#ifdef TXCHAIN_VERIFY_MODEL_VECTOR

		int idx = count % MAX_VECTOR_SIZE;

		if (_txv[idx].status != TXCHAIN_STATUS_VERIFIED)
		{
			usleep(10);
			continue;
		}

		string data = _txv[idx].data;

#endif	// TXCHAIN_VERIFY_MODEL_VECTOR

		// WRITE 
		woptions = leveldb_writeoptions_create();
		leveldb_put(ldb, woptions, "key", 3, data.c_str(), data.length(), &err);

		if (err != NULL) {
			fprintf(stderr, "ERROR: Level DB write failed!\n");
			usleep(100);
			continue;
		}

		leveldb_free(err); err = NULL;

#ifdef TXCHAIN_VERIFY_MODEL_VECTOR

		_txv[idx].data.clear();
		_txv[idx].verified = -1;
		_txv[idx].status = TXCHAIN_STATUS_EMPTY;

#endif	// TXCHAIN_VERIFY_MODEL_VECTOR

		count++;
		if (count % 100000 == 0)
		{
			printf("Level DB: %d\n", count);
		}
	}

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
