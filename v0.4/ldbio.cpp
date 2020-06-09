#include "txcommon.h"


leveldb	_systemdb;
leveldb	_txdb;
leveldb	_walletdb;


int	db_process(txdata_t& txdata);


void	*thread_levledb(void *info_p)
{
	int	chainport = *(int *)info_p;
	int	count = 0;
	char	dbname[256] = {0};


	mkdir("db", 0755);

	sprintf(dbname, "db/system-%d.db", chainport);
	_systemdb.open(dbname);

	sprintf(dbname, "db/tx-%d.db", chainport);
	_txdb.open(dbname);

	sprintf(dbname, "db/wallet-%d.db", chainport);
	_walletdb.open(dbname);


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

	_systemdb.close();
	_txdb.close();
	_walletdb.close();

	return 0;
}


int	db_process(txdata_t& txdata)
{
	string	key = txdata.hdr.txid;

	printf("    Save: key=%s  / data length=%ld\n", key.c_str(), txdata.bodyser.size());

	_walletdb.put(key, txdata.bodyser);

	return 1;
}
