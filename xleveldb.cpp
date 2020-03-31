#include <iostream>
#include <leveldb/c.h>
#include <stdio.h>

#include "xleveldb.h"
#include "xmsq.h"

using namespace std;

void *thread_exleveldb(void *info_p)
{
	leveldb_t *db;
	leveldb_options_t *options;
	leveldb_writeoptions_t *woptions;
	char *err = NULL;

	int rmsqid = -1;
	data_t msq_data;

		// message queue connect
	rmsqid = msgget( (key_t)1235, IPC_CREAT | 0666);
	if (rmsqid == -1) {
	}

	/******************************************/
	/* OPEN */

	options = leveldb_options_create();
	leveldb_options_set_create_if_missing(options, 1);
	db = leveldb_open(options, "testdb", &err);

	if (err != NULL) {
		fprintf(stderr, "Open fail.\n");
		return 0;
	}

	/* reset error var */
	leveldb_free(err); err = NULL;

	while (1) {

		// message queue recv
		if (msgrcv(rmsqid, &msq_data,
					sizeof(msq_data), 1, 0) == -1) {
			printf("message queue recv error\n");
		}

		std::string data(msq_data.mtext);

		/******************************************/
		/* WRITE */

		woptions = leveldb_writeoptions_create();
		leveldb_put(db, woptions, "key", 3, "value", 5, &err);

		if (err != NULL) {
			fprintf(stderr, "Write fail.\n");
			return 0;
		}

		leveldb_free(err); err = NULL;

	}

	/******************************************/
	/* CLOSE */

	leveldb_close(db);

	/******************************************/
	/* DESTROY */

	leveldb_destroy_db(options, "testdb", &err);

	if (err != NULL) {
		fprintf(stderr, "Destroy fail.\n");
		return 0;
	}

	leveldb_free(err); err = NULL;


	return 0;
}
