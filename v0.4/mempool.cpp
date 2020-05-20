#include "txcommon.h"


vector<txdata_t>	_mempool;		// mempool
size_t			_mempool_count = 0;	// number of mempool
map<string, size_t>	_mempoolmap;		// mempool index (key=txid)
mutex			_mempool_lock;		// mempool lock


int	mempool_process(txdata_t& txdata);
int	make_block(txdata_t& txdata);


int	mempool_add(txdata_t& txdata)
{
	string	key;

	key = txdata.hdr.txid;
	assert(key.size() >= 32);

	_mempool_lock.lock();

	if (_mempool_count >= _mempool.size() - 1)
	{
		printf("MEMPOOL RESIZE %ld => %ld\n", _mempool.size(), _mempool.size() + 1000);
		_mempool.resize(_mempool.size() + 1000);
	}

	_mempool[_mempool_count] = txdata;

	_mempoolmap[key] = _mempool_count;	// index는 1부터 시작함 (리턴시 값이 없는 것은 0이 리턴되므로)

	if (_debug > 1) printf("    Mempool add[%ld] = %s\n", _mempool_count, key.c_str());
	_mempool_count++;

	if (_mempool.size() - _mempool_count > 1000)
	{
		printf("MEMPOOL SHRINK %ld => %ld\n",
			_mempool.size(), _mempool_count + 1000);

		_mempool.resize(_mempool_count + 1000);
		_mempool.shrink_to_fit();
	}

	_mempool_lock.unlock();

	return 1;
}


void	mempool_update(string txid, int flag)
{
	lock_guard<mutex> lock(_mempool_lock);

	if (_mempoolmap.count(txid) <= 0)
		return;

	size_t idx = _mempoolmap[txid];
	txdata_t& curtxdata = _mempool[idx];

	curtxdata.hdr.flag |= flag;

//	_mempool_lock.unlock();
}
