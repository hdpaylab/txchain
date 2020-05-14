#include "txcommon.h"


leveldb	_systemdb;
leveldb	_walletdb;


void	*thread_levledb(void *info_p)
{
	int	chainport = *(int *)info_p;
	int	count = 0;
	char	dbname[256] = {0};
	double	tmstart, tmend;

	sprintf(dbname, "system-%d.db", chainport);
	_systemdb.open(dbname);

	sprintf(dbname, "wallet-%d.db", chainport);
	_walletdb.open(dbname);

	tmstart = xgetclock();

	while (1)
	{
		txdata_t txdata;
		tx_send_token_t txsend;
		xserial txsz(4 * 1024);
		string	key;

		count++;
		txdata = _mempoolq.pop();

		txsz.setstring(txdata.data);
		printf("\n");
		printf("-----LDBIO:\n");
		deseriz(txsz, txsend, 0);
		deseriz(txsz, txdata.sign, 0);

		key = txdata.sign.signature;

		_walletdb.put(key, txdata.data);

#ifdef DEBUG
#else
		if (count % 100000 == 0)
#endif
			printf("LDB : Recv %7d veriq=%5ld\n", count, _mempoolq.size());
		if (count >= MAX_TEST_NUM_TX)
			break;
	}

	tmend = xgetclock();
	printf("LDB : Recv time=%.3f / %.1f/sec\n",
		tmend - tmstart, count / (tmend - tmstart));

	_systemdb.close();
	_walletdb.close();

	return 0;
}
