#include "txcommon.h"


map<string, txdata_t> _mempoolmap;	// mempool index (key=txid)
mutex _mempool_lock;			// mempool lock


int	mempool_add(txdata_t& txdata)
{
	string	txid;

	txid = txdata.hdr.txid;
	assert(txid.size() >= 32);

	_mempool_lock.lock();

	_mempoolmap[txid] = txdata;

	logprintf(1, "    Mempool add [%s] = %s %s flag=0x%08X\n", 
		txid.c_str(), get_type_name(txdata.hdr.type), 
		txdata.hdr.status > 0 ? get_status_name(txdata.hdr.status) : "", txdata.hdr.flag);

	_mempool_lock.unlock();

	return 1;
}


void	mempool_update(string txid, int flag)
{
	lock_guard<mutex> lock(_mempool_lock);

	if (_mempoolmap.count(txid) <= 0)
	{
		logprintf(1, "WARNING: txid not found in mempool. txid=%s\n", txid.c_str());
		return;
	}

	logprintf(1, "    Mempool update txid=%s flag|=0x%08X\n", txid.c_str(), flag);

	txdata_t& curtxdata = _mempoolmap[txid];

	curtxdata.hdr.flag |= flag;

//	_mempool_lock.unlock();
}
