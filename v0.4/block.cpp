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


int	block_gen_prepare();
void	send_block_gen(txdata_t& txdata);	// BLOCK 생성 명령 발송 


//
// 일정 주기마다 블록 생성 요청을 보내서 합의를 받은 다음 블록 생성함 
//
void	*thread_block_gen(void *info_p)
{
	int	chainport = *(int *)info_p;
	double	blocktime = xgetclock();

	_block_gen.on_air = 0;
	_block_gen.block_height = _block_height;
	_block_gen.sign_hash = string();
	_block_gen.ntx = 0;

	while (1)
	{
		if (xgetclock() - blocktime < 5 || _mempool_count <= 0 || _block_gen.on_air == 1)
		{
			if (_debug > 4) printf("%.3f %ld %d\n", 
					xgetclock() - blocktime, _mempool_count, _block_gen.on_air);
			sleepms(1000);
			continue;
		}

		int ntx = block_gen_prepare();
		if (ntx <= 0)
		{
			sleepms(100);
			continue;
		}

		printf("MAKE BLOCK: %d tx sync request...\n", ntx);

		blocktime = xgetclock();

#ifdef DEBUG
#else
		if (count % 100000 == 0)
			printf("    mempool processed %d mempoolq=%5ld\n", count, _mempoolq.size());
#endif
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
int	block_gen_prepare()
{
	double	curclock = xgetclock();

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
		if (curclock - txdata.hdr.recvclock < 0.1)	// 수신한지 0.1초 지나지 않은 경우 skip
			continue;

		if (_debug > 3) printf("BLOCK: prepare %d diff=%.3f %s\n", 
				_block_gen.ntx + 1, curclock - txdata.hdr.recvclock, txdata.hdr.txid.c_str());

		txdata.hdr.flag |= FLAG_TX_LOCK;	// TX LOCK

		_block_gen.mempoolidx[_block_gen.ntx] = nn;	// mempool index
		_block_gen.ntx++;
		if (_block_gen.ntx >= 100000)
			break;
	}

	_mempool_lock.unlock();

	if (_block_gen.ntx <= 0)
	{
		_block_gen.on_air = 0;
		return 0;
	}
	printf("BLOCK_PREPARE: %d tx ready\n", _block_gen.ntx);

	// 블록 내용에 맞게 크기 조정
//	_block_gen.mempoolidx.resize(_block_gen.ntx);
//	_block_gen.mempoolidx.shrink_to_fit();


	txdata_t newtxdata;
	xserialize hdrszr, bodyszr;
	tx_header_t hdr;

	// 바디 serialization
	_mempool_lock.lock();

	for (int nn = 0; nn < _block_gen.ntx; nn++)
	{
		txdata_t& curtxdata = _mempool[nn];
		tx_block_gen_req_t block_txid;

		block_txid.txid = curtxdata.hdr.txid;
		seriz_add(bodyszr, block_txid);
	}

	_mempool_lock.unlock();

	// 헤더 serialization
	hdr.nodeid = getpid();	// 임시로 
	hdr.block_height = _block_height;	// 다음 생성할 블록 번호 
	hdr.type = TX_BLOCK_GEN_REQ;
	hdr.data_length = bodyszr.size();
	hdr.from_addr = from_addr;
	hdr.txclock = xgetclock();
	hdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	seriz_add(hdrszr, hdr);

	printf("    BLOCK_PREPARE: signature: %s\n", hdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(from_addr, hdr.signature.c_str(), 
				bodyszr.data(), bodyszr.size(), &_netparams.AddrHelper);
	printf("    BLOCK_PREPARE: verify_check=%d\n", verify_check);
	printf("\n");

	newtxdata.hdr = hdr;
	newtxdata.hdrser = hdrszr.getstring();
	newtxdata.bodyser = bodyszr.getstring();

	// 발송 
	_sendq.push(newtxdata);

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
	xserialize bodyszr, newbodyszr;

	hp = &txdata.hdr;
	bodyszr.setstring(txdata.bodyser);

	// TX 검증 
	hp->valid = verify_message_bin(hp->from_addr.c_str(), hp->signature.c_str(), 
				txdata.bodyser.c_str(), hp->data_length, &_netparams.AddrHelper);
	hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";
	printf("    BLOCK_GEN_REQ verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());

	// 검증 실패하면 폐기 
	if (hp->valid == 0)
	{
		fprintf(stderr, "ERROR: BLOCK_GEN_REQ verification failed: nodeid=%d txid=%s\n",
			hp->nodeid, hp->txid.c_str());
		return;
	}

	int	ntotal = 0, nfail = 0;

	_mempool_lock.lock();

	while (1)
	{
		tx_block_gen_req_t block_gen;

		// TXID 추출 
		int ret = deseriz(bodyszr, block_gen, 0);
		if (ret == 0)
			break;
		ntotal++;

		// mempool에서 해당 TXID 검사 
		if (_mempoolmap.count(block_gen.txid) <= 0)
		{
			printf("WARNING: BLOCK_GEN_REQ: TXID NOT FOUND: %s\n", block_gen.txid.c_str());
			nfail++;
			continue;
		}

		size_t idx = _mempoolmap[block_gen.txid];
		txdata_t& curtxdata = _mempool[idx];

		curtxdata.hdr.flag |= FLAG_TX_LOCK;
		if (_debug > 2) printf("    BLOCK_GEN_REQ: %d TX_LOCK TXID=%s\n", ntotal, block_gen.txid.c_str());
	}

	_mempool_lock.unlock();

	printf("    BLOCK_GEN_REQ ntotal=%d nfail=%d\n", ntotal, nfail);

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
	hdr.from_addr = from_addr;
	hdr.txclock = xgetclock();
	hdr.signature = sign_message_bin(privkey, newbodyszr.data(), newbodyszr.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	xserialize hdrszr;
	seriz_add(hdrszr, hdr);

	newtxdata.hdr = hdr;
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
	hdr.data_length = bodyszr.size();
	hdr.from_addr = from_addr;
	hdr.txclock = xgetclock();
	hdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	seriz_add(hdrszr, hdr);

	newtxdata.hdr = hdr;
	newtxdata.hdrser = hdrszr.getstring();
	newtxdata.bodyser = bodyszr.getstring();

	printf("    Add to sendq(TX_BLOCK_GEN): type=%s sign_hash=%s\n",
		get_type_name(hdr.type), cmd.sign_hash.c_str());

	_sendq.push(newtxdata);	// broadcast to other nodes... (request verification)


	// 블록 생성 
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

	printf("BLOCK_GEN: mempool_count=%ld\n", _mempool_count);

	// 실제 블록 생성: 전체 TX 목록에 대해서 sign 후 저장 (이전 블록 hash 필요)
	_mempool_lock.lock();

	int ntx = 0;
	for (ssize_t ii = 0; ii < (ssize_t) _mempool_count; ii++)
	{
		txdata_t& curtxdata = _mempool[ii];

		if (curtxdata.hdr.flag & FLAG_TX_LOCK)
		{
			xserialize hdrszr;

			curtxdata.hdr.status = 0;
			curtxdata.hdr.valid = 1;
			curtxdata.hdr.txid = string();

			seriz_add(hdrszr, curtxdata.hdr);
			curtxdata.hdrser = hdrszr.getstring();	// 헤더 교체 

			block_txlist += curtxdata.hdrser + curtxdata.bodyser;	// 원본 serialized 데이터를 연결 

			curtxdata.hdr.flag |= FLAG_TX_LOCK;	// 삭제할 것

			ntx++;
		//	if (_debug > 2)
				printf("    Block [%ld] %d hdrsz=%ld bodysz=%ld value=%d\n", 
					block_height, ntx, curtxdata.hdrser.size(), 
					curtxdata.bodyser.size(), curtxdata.hdr.value);
		}
		else
		{
			printf("WARNING: mempool [%ld] value=%d flag=%08X\n", 
				ii, curtxdata.hdr.value, curtxdata.hdr.flag);
		}
	}

	int ndel = 0;
	for (ssize_t ii = _mempool_count; ii >= 0; ii--)
	{
		txdata_t& curtxdata = _mempool[ii];

		if (curtxdata.hdr.flag & FLAG_TX_LOCK)
		{
			if (_debug > 2) printf("    Mempool %ld 삭제: txid=%s\n", ii, curtxdata.hdr.txid.c_str());

			_mempoolmap.erase(curtxdata.hdr.txid);	// mempoolmap에서 제거 
			_mempool.erase(_mempool.begin() + ii);	// mempool에서 제거 
			_mempool_count--;
			ndel++;
		}
	}

	_mempool_lock.unlock();

	printf("    BLOCK_GEN: ntx=%d ndel=%d \n", ntx, ndel);

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
