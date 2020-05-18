#include "txcommon.h"


vector<txdata_t>	_mempool;	// mempool
size_t			_mempool_count = 0;	// number of mempool
map<string, txdata_t *>	_mempoolmap;	// mempool index (key=txid)


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


void	*thread_block_sync(void *info_p)
{
	int	chainport = *(int *)info_p;
	int	count = 0;
	double	blocktime = xgetclock();

	while (1)
	{
		if (xgetclock() - blocktime < 10 || _mempool_count <= 0)
		{
			sleepms(100);
			continue;
		}

		txdata_t txdata;

		int ntx = make_block(txdata);
		if (ntx <= 0)
		{
			sleepms(1);
			continue;
		}

		printf("MAKE BLOCK: %d tx sync request...\n", ntx);

		blocktime = xgetclock();

#ifdef DEBUG
#else
		if (count % 100000 == 0)
#endif
			printf("    mempool processed %d mempoolq=%5ld\n", count, _mempoolq.size());
	}

	pthread_exit(NULL);

	return 0;
}


void	update_tx_status(string txid, int flag)
{
	txdata_t *txp = _mempoolmap[txid];
	if (txp == NULL)
		return;

	txdata_t& txdata = *_mempoolmap[txid];

	txdata.hdr.flag |= flag;
}


static	const char *privkey = "LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm";	// privkey
static	const char *from_addr = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";	
static	const char *to_addr = "HUGUrwcFy1VC91nq7tRuZpaJqndoHDw64e";


//
// mempool에서 최근 들어온 TX 중에서 sendq로 발송된 것만 목록으로 만듬
// 각 노드로 보내서 블록 생성 합의 요청
//
int	make_block(txdata_t& txdata)
{
	vector<string> txidlist;
	int ntxidlist = 0;

	txidlist.resize(10000);

	for (int nn = 0; nn < (ssize_t)_mempool_count; nn++)
	{
		txdata_t txdata = _mempool[nn];

		if (!(txdata.hdr.flag & FLAG_SENT_TX))	// 아직 동기화를 위해서 발송하지 않은 경우 
			continue;

		printf("BLOCK: add %d:%s\n", ntxidlist, txdata.hdr.txid.c_str());
		txidlist[ntxidlist] = txdata.hdr.txid;
		ntxidlist++;
	}

	if (ntxidlist <= 0)
		return ntxidlist;

	txidlist.resize(ntxidlist);
	txidlist.shrink_to_fit();

	txdata.hdr.nodeid = getpid();	// 임시로 
	txdata.hdr.type = TX_BLOCK_SYNC_REQ;
	txdata.hdr.status = 0;
	txdata.hdr.valid = -1;
	txdata.hdr.txid = string();
	txdata.hdr.from_addr = from_addr;
	txdata.hdr.txclock = xgetclock();
	txdata.hdr.flag = 0;

	xserialize hdrszr, bodyszr;

	for (int nn = 0; nn < ntxidlist; nn++)
	{
		tx_block_txid_t	block_txid;

		block_txid.txid = txidlist[nn];
		seriz_add(bodyszr, block_txid);
	}
	txdata.hdr.data_length = bodyszr.size();

	txdata.hdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), 
				&_params.PrivHelper, &_params.AddrHelper);
	printf("    block sync: signature: %s\n", txdata.hdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(from_addr, txdata.hdr.signature.c_str(), 
				bodyszr.data(), bodyszr.size(), &_params.AddrHelper);
	printf("    verify_check=%d\n", verify_check);
	printf("\n");

	// sign은 verify 테스트 전에 serialize하면 안됨..
	seriz_add(hdrszr, txdata.hdr);

	txdata.hdrser = hdrszr.getstring();
	txdata.bodyser = bodyszr.getstring();

	_sendq.push(txdata);

	printf("Broadcast BLOCK SYNC...\n");

	return ntxidlist;
}
