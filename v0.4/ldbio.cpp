#include "txcommon.h"


leveldb	_systemdb;
leveldb	_walletdb;


int	db_process(txdata_t& txdata);


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

		count++;
		txdata = _leveldbq.pop();

		printf("\n-----LevelDB:\n");

		db_process(txdata);

#ifdef DEBUG
#else
		if (count % 100000 == 0)
#endif
			printf("    leveldb processed %d mempoolq=%5ld\n", count, _mempoolq.size());
	}

	tmend = xgetclock();
	printf("LDB : Recv time=%.3f / %.1f/sec\n",
		tmend - tmstart, count / (tmend - tmstart));

	_systemdb.close();
	_walletdb.close();

	return 0;
}


int	db_process(txdata_t& txdata)
{
	tx_send_token_t txsend;
	xserialize hdrszr, bodyszr;
	string	key;

	key = txdata.hdr.txid;

	printf("    Save: key=%s  / data length=%ld\n", key.c_str(), txdata.bodyser.size());

	_walletdb.put(key, txdata.bodyser);

	return 1;
}
