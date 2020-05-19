#include "txcommon.h"


typedef struct {
	int	on_air;				// 현대 블록 생성 명령 작동 중이면 1
	size_t	block_height;			// 블록 번호 
	string	sign_hash;			// block_gen_req로 보낸 signature의 hash 값 
	vector<size_t> mempoolidx;		// mempool index
	int	ntx;				// number of tx
}	last_block_gen_t;


size_t	_block_height = 1;			// 다음 생성할 블록 height

last_block_gen_t	_block_gen;

map<uint32_t, int>	_block_gen_node;	// key=nodeid value=nfail


int	block_gen_prepare(txdata_t& txdata);
void	send_block_gen(txdata_t& txdata);	// BLOCK 생성 명령 발송 


//
// 일정 주기마다 블록 생성 요청을 보내서 합의를 받은 다음 블록 생성함 
//
void	*thread_block_gen(void *info_p)
{
	int	chainport = *(int *)info_p;
	int	count = 0;
	double	blocktime = xgetclock();

	_block_gen.on_air = 0;
	_block_gen.block_height = _block_height;
	_block_gen.sign_hash = string();
	_block_gen.ntx = 0;

	while (1)
	{
		if (xgetclock() - blocktime < 5 || _mempool_count <= 0 || _block_gen.on_air == 1)
		{
			sleepms(100);
			continue;
		}

		txdata_t txdata;

		int ntx = block_gen_prepare(txdata);
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


// 임시 변수 
static	const char *privkey = "LU1fSDCGy3VmpadheAu9bnR23ABdpLQF2xmUaJCMYMSv2NWZJTLm";	// privkey
static	const char *from_addr = "HRg2gvQWX8S4zNA8wpTdzTsv4KbDSCf4Yw";	
static	const char *to_addr = "HUGUrwcFy1VC91nq7tRuZpaJqndoHDw64e";


//
// mempool에서 최근 들어온 TX 중에서 sendq로 발송된 것만 목록으로 만듬
// 각 노드로 보내서 블록 생성 합의 요청함
//
int	block_gen_prepare(txdata_t& txdata)
{
	_block_gen.on_air = 1;

	// 최대 10만개 처리 가능
	_block_gen.mempoolidx.resize(100000);

	_mempool_lock.lock();
	for (int nn = 0; nn < (ssize_t)_mempool_count; nn++)
	{
		txdata_t& txdata = _mempool[nn];

		if (txdata.hdr.flag & FLAG_TX_LOCK)	// 다른 노드에서 블록 생성하기 위해 TX 잠근 경우 
			continue;
		if (!(txdata.hdr.flag & FLAG_SENT_TX))	// 아직 동기화를 위해서 발송하지 않은 경우 
			continue;

		printf("BLOCK: add %d:%s\n", _block_gen.ntx, txdata.hdr.txid.c_str());

		txdata.hdr.flag |= FLAG_TX_LOCK;	// TX LOCK

		_block_gen.mempoolidx[_block_gen.ntx] = nn;	// mempool index
		_block_gen.ntx++;
		if (_block_gen.ntx >= 100000)
			break;
	}
	_mempool_lock.unlock();

	if (_block_gen.ntx <= 0)
		return 0;

	_block_gen.mempoolidx.resize(_block_gen.ntx);
	_block_gen.mempoolidx.shrink_to_fit();


	xserialize hdrszr, bodyszr;
	tx_header_t hdr;

	// 바디 serialization
	_mempool_lock.lock();
	for (int nn = 0; nn < _block_gen.ntx; nn++)
	{
		txdata_t& txdata = _mempool[nn];
		tx_block_gen_req_t block_txid;

		block_txid.txid = txdata.hdr.txid;
		seriz_add(bodyszr, block_txid);
	}
	_mempool_lock.unlock();

	// 헤더 serialization
	hdr.nodeid = getpid();	// 임시로 
	hdr.block_height = _block_height;	// 다음 생성할 블록 번호 
	hdr.type = TX_BLOCK_GEN_REQ;
	hdr.status = 0;
	hdr.data_length = bodyszr.size();
	hdr.valid = -1;
	hdr.txid = string();
	hdr.from_addr = from_addr;
	hdr.txclock = xgetclock();
	hdr.flag = 0;
	hdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);

	seriz_add(hdrszr, hdr);

	printf("    block sync: signature: %s\n", hdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(from_addr, hdr.signature.c_str(), 
				bodyszr.data(), bodyszr.size(), &_netparams.AddrHelper);
	printf("    BLOCK_GEN: verify_check=%d\n", verify_check);
	printf("\n");

	txdata.hdrser = hdrszr.getstring();
	txdata.bodyser = bodyszr.getstring();

tx_header_t tmphdr;
deseriz(hdrszr, tmphdr, 1);

	_sendq.push(txdata);

	_block_gen.block_height = _block_height;
	_block_gen.sign_hash = sha256(hdr.signature);

	return _block_gen.ntx;
}


//
// BLOCK_GEN_REQ 요청 처리: 해당 TX 검증 => 각 TXID 존재여부 확인 => 이상 없으면 OK 리턴 
// 다른 노드에서 블록을 생성하겠다고 한 경우에 여기로 옴
//
void	ps_block_gen_req(txdata_t& txdata)
{
	tx_header_t *hp;
	xserialize hdrszr, bodyszr, newbodyszr;

	hp = &txdata.hdr;
	bodyszr.setstring(txdata.bodyser);

	// TX 검증 
	hp->valid = verify_message_bin(hp->from_addr.c_str(), hp->signature.c_str(), 
				txdata.bodyser.c_str(), hp->data_length, &_netparams.AddrHelper);
	hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
	printf("    BLOCK_GEN verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());

	// 검증 실패하면 폐기 
	if (hp->valid == 0)
	{
		fprintf(stderr, "ERROR: BLOCK_GEN verification failed: nodeid=%d txid=%s\n",
			hp->nodeid, hp->txid.c_str());
		return;
	}

	int	ntotal = 0, nfail = 0;

	while (1)
	{
		tx_block_gen_req_t block_gen;

		// TXID 추출 
		int ret = deseriz(bodyszr, block_gen, 0);
		if (ret == 0)
			break;
		ntotal++;

		// mempool에서 해당 TXID 검사 
		_mempool_lock.lock();
		txdata_t *txp = _mempoolmap[block_gen.txid];
		if (txp == NULL)
		{
			printf("WARNING: BLOCK_GEN: TXID NOT FOUND: %s\n", block_gen.txid.c_str());
			nfail++;
		}
		else
		{
			txdata_t& txdata = *_mempoolmap[block_gen.txid];
			txdata.hdr.flag |= FLAG_TX_LOCK;
			printf("    BLOCK_GEN: Marked as next block TXID=%s\n", block_gen.txid.c_str());
		}
		_mempool_lock.unlock();
	}

	txdata_t newtxdata;
	tx_header_t hdr;
	tx_block_gen_reply_t reply;

	// Body serialize
	reply.ntotal = ntotal;
	reply.nfail = nfail;

	seriz_add(newbodyszr, reply);

	// Header serialize
	hdr.nodeid = getpid();
	hdr.type = TX_BLOCK_GEN_REPLY;
	hdr.status = -nfail;
	hdr.data_length = newbodyszr.size();
	hdr.valid = -1;
	hdr.txid = "";
	hdr.from_addr = from_addr;
	hdr.txclock = xgetclock();
	hdr.flag = 0;
	hdr.signature = sign_message_bin(privkey, newbodyszr.data(), newbodyszr.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	seriz_add(hdrszr, hdr);

	newtxdata.hdrser = hdrszr.getstring();
	newtxdata.bodyser = newbodyszr.getstring();

	printf("    Add to sendq(TX_BLOCK_GEN_REPLY): type=%s nfail=%d\n",
		get_type_name(hdr.type), hdr.status);

	_sendq.push(newtxdata);	// broadcast to other nodes... (request verification)
}


//
// BLOCK_GEN 응답 
//
void	ps_block_gen_reply(txdata_t& txdata)
{
	xserialize bodyszr;
	tx_header_t *hp = &txdata.hdr;
	tx_block_gen_reply_t reply;

	bodyszr.setstring(txdata.bodyser);
	deseriz(bodyszr, reply);

	printf("    TX_BLOCK_GEN_REPLY: fail=%d\n", hp->status);

	hp->valid = verify_message_bin(hp->from_addr.c_str(), hp->signature.c_str(), 
				txdata.bodyser.c_str(), hp->data_length, &_netparams.AddrHelper);
	if (hp->valid)
	{
		int	nok = 0;

		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";

		_block_gen_node[hp->nodeid] = reply.nfail;

		// 일정 비율의 노드가 합의한 경우, 블록 생성 명령 발송 
		for (map<uint32_t,int>::iterator it=_block_gen_node.begin(); it != _block_gen_node.end(); ++it)
		{
			printf("    MAP [%u] => %d\n", it->first, it->second);
			if (it->second <= 0)
				nok++;
		}

		printf("    TX_BLOCK_GEN_REPLY verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());
		printf("    ntotal=%d nfail=%d\n", reply.ntotal, reply.nfail);

		if (nok >= 1)
		{
			send_block_gen(txdata);
		}
	}
	else
	{
		printf("    TX_BLOCK_GEN_REPLY verify FAILED=%d \n", hp->valid);
	}
}


//
// 각 노드에 블록 생성 명령 전송 
//
void	send_block_gen(txdata_t& txdata)
{
	txdata_t newtxdata;
	tx_header_t hdr;
	tx_block_gen_t cmd;
	xserialize hdrszr, bodyszr;

	// Body serialize
	cmd.sign_hash = _block_gen.sign_hash;

	seriz_add(bodyszr, cmd);

	// Header serialize
	hdr.nodeid = getpid();
	hdr.block_height = _block_height;	// 다음 생성할 블록 번호 
	hdr.type = TX_BLOCK_GEN;
	hdr.status = 0;
	hdr.data_length = bodyszr.size();
	hdr.valid = -1;
	hdr.txid = "";
	hdr.from_addr = from_addr;
	hdr.txclock = xgetclock();
	hdr.flag = 0;
	hdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	seriz_add(hdrszr, hdr);

	newtxdata.hdrser = hdrszr.getstring();
	newtxdata.bodyser = bodyszr.getstring();

	printf("    Add to sendq(TX_BLOCK_GEN): type=%s sign_hash=%s\n",
		get_type_name(hdr.type), cmd.sign_hash.c_str());

	_sendq.push(newtxdata);	// broadcast to other nodes... (request verification)


	ps_block_gen(txdata);
}


//
// 실제 블록 생성 
//
void	ps_block_gen(txdata_t& txdata)
{
	string	block_txlist;		// 블록의 TX 목록 
	xserialize blockszr;

	size_t block_height = txdata.hdr.block_height;

	// 실제 블록 생성: 전체 TX 목록에 대해서 sign 후 저장 (이전 블록 hash 필요)
	_mempool_lock.lock();
	for (ssize_t ii = 0; ii < (ssize_t) _mempool.size(); ii++)
	{
		txdata_t& txdata = _mempool[ii];

		if (txdata.hdr.flag & FLAG_TX_LOCK)
		{
			xserialize hdrszr;

			txdata.hdr.status = 0;
			txdata.hdr.valid = 1;
			txdata.hdr.txid = string();
			txdata.hdr.flag = 0;			// 헤더 초기화 

			seriz_add(hdrszr, txdata.hdr);
			txdata.hdrser = hdrszr.getstring();	// 헤더 교체 

			block_txlist += txdata.hdrser + txdata.bodyser;	// 원본 serialized 데이터를 연결 

			txdata.hdr.flag |= FLAG_TX_LOCK;	// 삭제할 것

			printf("    Block [%ld] %ld hdrsz=%ld bodysz=%ld\n", 
				block_height, ii, txdata.hdrser.size(), txdata.bodyser.size());
		}
	}

	for (ssize_t ii = _mempool_count - 1; ii >= 0; ii--)
	{
		txdata_t& txdata = _mempool[ii];

		if (txdata.hdr.flag & FLAG_TX_LOCK)
		{
			printf("    Mempool %ld 삭제: txid=%s\n", ii, txdata.hdr.txid.c_str());

			_mempoolmap.erase(txdata.hdr.txid);	// mempoolmap에서 제거 
			_mempool.erase(_mempool.begin() + ii);	// mempool에서 제거 
			_mempool_count--;

			txdata.hdr.flag = 0;
		}
	}
	_mempool_lock.unlock();

	// 블록 정보 setup
	block_info_t block_info;

	memset(block_info.block_hash, 0, sizeof(block_info.block_hash));
	block_info.block_height = block_height;
	block_info.gen_addr = from_addr;
	block_info.signature = sign_message_bin(privkey, block_txlist.c_str(), block_txlist.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	block_info.prev_block_hash = sha256("이전 블록 hash");
	block_info.block_clock = xgetclock();
	block_info.ntx = _block_gen.ntx;

	string txhash = sha256(block_txlist);
	seriz_add(blockszr, block_info);
	string infohash = sha256(blockszr.getstring());
	string blockhash = sha256(infohash + txhash);

	memcpy(block_info.block_hash, blockhash.c_str(), blockhash.size());

	seriz_add(blockszr, block_info);

	string block_data = blockszr.data() + block_txlist;
	printf("++++++++++블록 생성 완료: %ld 파일 출력 필요\n", block_data.size());

	// 다음 블록으로..
	_block_height = block_height + 1;

	_block_gen.mempoolidx.clear();
	_block_gen.ntx = 0;
	_block_gen.on_air = 0;
}
