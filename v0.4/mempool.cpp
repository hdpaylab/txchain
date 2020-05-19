#include "txcommon.h"


vector<txdata_t>	_mempool;		// mempool
size_t			_mempool_count = 0;	// number of mempool
map<string, txdata_t *>	_mempoolmap;		// mempool index (key=txid)


int	mempool_process(txdata_t& txdata);
int	make_block(txdata_t& txdata);


int	add_mempool(txdata_t& txdata)
{
	string	key;

	key = txdata.hdr.txid;

	if (_mempool_count >= _mempool.size() - 1)
	{
		printf("MEMPOOL RESIZE %ld => %ld\n", _mempool.size(), _mempool.size() + 1000);
		_mempool.resize(_mempool.size() + 1000);
	}

	_mempool[_mempool_count] = txdata;

	_mempoolmap[key] = &_mempool[_mempool_count];

	printf("    Mempool add[%ld] = %s\n", _mempool_count, key.c_str());
	_mempool_count++;

	if (_mempool.size() - _mempool_count > 1000)
	{
		printf("MEMPOOL SHRINK %ld => %ld\n",
			_mempool.size(), _mempool_count + 1000);
		_mempool.resize(_mempool_count + 1000);
		_mempool.shrink_to_fit();
	}

//	printf("    Mempool num=%ld size=%ld: key=%s  / data length=%ld\n", 
//		_mempool_count, _mempool.size(), key.c_str(), txdata.bodyser.size());

	return 1;
}


void	update_tx_status(string txid, int flag)
{
	txdata_t *txp = _mempoolmap[txid];
	if (txp == NULL)
		return;

	txdata_t& txdata = *_mempoolmap[txid];

	txdata.hdr.flag |= flag;
}
