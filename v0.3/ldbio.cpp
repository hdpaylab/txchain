#include "txcommon.h"
#include "leveldb.h"


void	*thread_levledb(void *info_p)
{
	int	count = 0;
	double	tmstart, tmend;

	leveldb db("test.db");

	tmstart = xgetclock();

	while (1)
	{
		txdata_t txdata;
		tx_send_token_t txsend;
		xserial txsz(4 * 1024);
		string	key;

		count++;
		txdata = _veriq.pop();

		txsz.setstring(txdata.data);
		printf("\n");
		printf("LDBIO:\n");
		deseriz(txsz, txsend, 1);
		deseriz(txsz, txdata.sign, 1);

		key = txdata.sign.signature;

		db.put(key, txdata.data);

#ifdef DEBUG
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
