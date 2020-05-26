#include "txcommon.h"


#define BLOCK_ITV	0.1


size_t	_block_height = 1;			// 다음 생성할 블록 height

block_txid_info_t	_self_txid_info;		// 자체적으로 블록 생성을 위한 txidlist
block_txid_info_t	_recv_txid_info;	// 블록 생성을 위해서 다른 노드에서 보낸 txidlist

map<uint32_t, int>	_txid_reply_node;	// key=nodeid value=nfail

vector<string>	_next_block_txids;		// txid list of next block


size_t	block_gen_prepare();
void	send_block_gen_reply(txdata_t& txdata);	// BLOCK 생성 명령 발송 


//
// 일정 주기마다 블록 생성 요청을 보내서 합의를 받은 다음 블록 생성함 
//
void	*thread_txid_info(void *info_p)
{
	int	chainport = *(int *)info_p;
	double	blocktime = xgetclock();

	_self_txid_info.on_air = 0;
	_self_txid_info.block_height = _block_height;
	_self_txid_info.sign_hash = string();

	while (1)
	{
		if (xgetclock() - blocktime < BLOCK_ITV || _mempoolmap.size() <= 0 || _self_txid_info.on_air == 1)
		{
			logprintf(5, "    Block generation check: %.3f %ld %d\n", 
					xgetclock() - blocktime, _mempoolmap.size(), _self_txid_info.on_air);
			sleepms(100);
			continue;
		}

		int ntx = block_gen_prepare();
		if (ntx <= 0)
		{
			sleepms(100);
			continue;
		}

		logprintf(1, "MAKE BLOCK: %d tx sync request...\n", ntx);

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
size_t	block_gen_prepare()
{
	double	curclock = xgetclock();

	_self_txid_info.on_air = 1;

	// 최대 10만개 처리 가능
	_self_txid_info.txidlist.clear();

	_mempool_lock.lock();

	map<string, txdata_t>::iterator it;
	for (it = _mempoolmap.begin(); it != _mempoolmap.end(); it++)
	{
		string txid = it->first;
		txdata_t& txdata = it->second;

		if (txdata.hdr.flag & FLAG_TX_LOCK)	// 다른 노드에서 블록 생성하기 위해 TX 잠근 경우 
			continue;
		if (!(txdata.hdr.flag & FLAG_SENT_TX))	// 아직 동기화를 위해서 발송하지 않은 경우 
			continue;
		if (curclock - txdata.hdr.recvclock < TX_TIME_DIFF)	// 수신한지 0.1초 지나지 않은 경우 skip
			continue;

		txdata.hdr.flag |= FLAG_TX_LOCK;	// TX LOCK

		logprintf(1, "    prepare BLOCK: prepare %d diff=%.3f flag=0x%08X %s\n", 
				_self_txid_info.txidlist.size() + 1, curclock - txdata.hdr.recvclock, 
				txdata.hdr.flag, txdata.hdr.txid.c_str());

		_self_txid_info.txidlist.push_back(txid);
		if (_self_txid_info.txidlist.size() >= 100000)
			break;
	}

	_mempool_lock.unlock();

	if (_self_txid_info.txidlist.size() <= 0)
	{
		_self_txid_info.on_air = 0;
		return 0;
	}
	logprintf(1, "BLOCK_PREPARE: %d tx ready\n", _self_txid_info.txidlist.size());


	txdata_t newtxdata;
	xserialize hdrszr, bodyszr;
	tx_header_t hdr;

	// 바디 serialization
	_mempool_lock.lock();

	for (int nn = 0; nn < (ssize_t)_self_txid_info.txidlist.size(); nn++)
	{
		string txid = _self_txid_info.txidlist[nn];
		txid_info_req_t block_txid;

		block_txid.txid = txid;
		seriz_add(bodyszr, block_txid);

		logprintf(1, "    prepare BLOCK Serialize: txid=%s \n", txid.c_str());
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

	logprintf(1, "    BLOCK_PREPARE: signature: %s\n", hdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(from_addr, hdr.signature.c_str(), 
				bodyszr.data(), bodyszr.size(), &_netparams.AddrHelper);
	logprintf(1, "    BLOCK_PREPARE: verify_check=%d\n", verify_check);

	newtxdata.hdr = hdr;
	newtxdata.hdrser = hdrszr.getstring();
	newtxdata.bodyser = bodyszr.getstring();

	// 발송 
	_sendq.push(newtxdata);

	_self_txid_info.block_height = _block_height;
	_self_txid_info.sign_hash = sha256(hdr.signature);

	return _self_txid_info.txidlist.size();
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
	logprintf(1, "    BLOCK_GEN_REQ verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());

	// 검증 실패하면 폐기 
	if (hp->valid == 0)
	{
		fprintf(stderr, "ERROR: BLOCK_GEN_REQ verification failed: nodeid=%d txid=%s\n",
			hp->nodeid, hp->txid.c_str());
		return;
	}

	int	nfail = 0;

	_mempool_lock.lock();

	_recv_txid_info.txidlist.clear();

	while (1)
	{
		txid_info_req_t txid_info;

		// TXID 추출 
		int ret = deseriz(bodyszr, txid_info, 0);
		if (ret == 0)
			break;
		string txid = txid_info.txid;

		// mempool에서 해당 TXID 검사 
		if (_mempoolmap.count(txid) <= 0)
		{
			logprintf(1, "WARNING: BLOCK_GEN_REQ: TXID NOT FOUND: %s\n", txid.c_str());
			nfail++;
			continue;
		}

		txdata_t& curtxdata = _mempoolmap[txid];
		curtxdata.hdr.flag |= FLAG_TX_LOCK;

		_recv_txid_info.txidlist.push_back(txid);

		logprintf(2, "    BLOCK_GEN_REQ: %d TX_LOCK TXID=%s\n", 
			_recv_txid_info.txidlist.size(), txid.c_str());
	}

	_mempool_lock.unlock();

	logprintf(1, "    BLOCK_GEN_REQ ntotal=%d nfail=%d\n", 
		_recv_txid_info.txidlist.size(), nfail);

	txdata_t newtxdata;
	tx_header_t hdr;
	txid_info_reply_t reply;

	// Body serialize
	reply.ntotal = _recv_txid_info.txidlist.size();
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

	logprintf(1, "    Add to sendq(TX_BLOCK_GEN_REPLY): type=%s nfail=%d\n",
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
	txid_info_reply_t reply;

	bodyszr.setstring(txdata.bodyser);
	deseriz(bodyszr, reply);

	logprintf(1, "    TX_BLOCK_GEN_REPLY: fail=%d\n", hp->status);

	hp->valid = verify_message_bin(hp->from_addr.c_str(), hp->signature.c_str(), 
				txdata.bodyser.c_str(), hp->data_length, &_netparams.AddrHelper);
	if (hp->valid)
	{
		int	nok = 0;

		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";

		_txid_reply_node[hp->nodeid] = reply.nfail;

		// 일정 비율의 노드가 합의한 경우, 블록 생성 명령 발송 
		for (map<uint32_t,int>::iterator it=_txid_reply_node.begin(); it != _txid_reply_node.end(); ++it)
		{
			logprintf(1, "    MAP [%u] => %d\n", it->first, it->second);
			if (it->second <= 0)
				nok++;
		}

		logprintf(1, "    TX_BLOCK_GEN_REPLY verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());
		logprintf(1, "    ntotal=%d nfail=%d\n", reply.ntotal, reply.nfail);

		if (nok >= 1)
		{
			send_block_gen_reply(txdata);
		}
	}
	else
	{
		logprintf(1, "    TX_BLOCK_GEN_REPLY verify FAILED=%d \n", hp->valid);
	}
}


//
// 각 노드에 블록 생성 명령 전송 
//
void	send_block_gen_reply(txdata_t& txdata)
{
	txdata_t newtxdata;
	tx_header_t hdr;
	tx_txid_info_t cmd;
	xserialize hdrszr, bodyszr;

	// Body serialize
	cmd.sign_hash = _self_txid_info.sign_hash;

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

	logprintf(1, "    Add to sendq(TX_BLOCK_GEN): type=%s sign_hash=%s\n",
		get_type_name(hdr.type), cmd.sign_hash.c_str());

	_sendq.push(newtxdata);	// broadcast to other nodes... (request verification)


	// 블록 생성 
	ps_block_gen(txdata, _self_txid_info);
}


//
// 실제 블록 생성 
//
void	ps_block_gen(txdata_t& txdata, block_txid_info_t& txid_info)
{
	string	block_txlist;		// 블록의 TX 목록 
	xserialize blockszr;

	txid_info.block_height = txdata.hdr.block_height;

	logprintf(1, "BLOCK_GEN: mempool size=%ld\n", _mempoolmap.size());

	// 실제 블록 생성: 전체 TX 목록에 대해서 sign 후 저장 (이전 블록 hash 필요)
	_mempool_lock.lock();

	int ntx = 0;
	for (ssize_t ii = 0; ii < (ssize_t)txid_info.txidlist.size(); ii++)
	{
		string txid = txid_info.txidlist[ii];
		txdata_t& curtxdata = _mempoolmap[txid];

		if (curtxdata.hdr.flag & FLAG_TX_LOCK)
		{
			xserialize hdrszr;

			curtxdata.hdr.status = 0;
			curtxdata.hdr.valid = 1;
			curtxdata.hdr.txid = string();

			seriz_add(hdrszr, curtxdata.hdr);
			curtxdata.hdrser = hdrszr.getstring();	// 헤더 교체 

			block_txlist += curtxdata.hdrser + curtxdata.bodyser;	// 원본 serialized 데이터를 연결 

			curtxdata.hdr.flag |= FLAG_TX_DELETE;	// 삭제할 것

			ntx++;
			logprintf(2, "    Block [%ld] %d hdrsz=%ld bodysz=%ld value=%d\n", 
				txid_info.block_height, ntx, curtxdata.hdrser.size(), 
				curtxdata.bodyser.size(), curtxdata.hdr.value);
		}
		else
		{
			logprintf(5, "    Skipped: mempool [%ld] txid=%s value=%d flag=%08X\n", 
				ii, curtxdata.hdr.txid.c_str(), curtxdata.hdr.value, curtxdata.hdr.flag);
		}
	}

	int ndel = 0;
	for (ssize_t ii = 0; ii < (ssize_t)txid_info.txidlist.size(); ii++)
	{
		string txid = txid_info.txidlist[ii];
		txdata_t& curtxdata = _mempoolmap[txid];

		if (curtxdata.hdr.flag & FLAG_TX_DELETE)
		{
			logprintf(2, "    Mempool %ld 삭제: txid=%s\n", ii, curtxdata.hdr.txid.c_str());

			_mempoolmap.erase(txid);	// mempoolmap에서 제거 
			ndel++;
		}
	}

	_mempool_lock.unlock();

	logprintf(1, "    BLOCK_GEN: ntx=%d ndel=%d \n", txid_info.txidlist.size(), ndel);

	// 블록 정보 setup
	block_info_t block_info;

	memset(block_info.block_hash, 0, sizeof(block_info.block_hash));
	block_info.block_height = txid_info.block_height;
	block_info.gen_addr = from_addr;
	block_info.signature = sign_message_bin(privkey, block_txlist.c_str(), block_txlist.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	block_info.prev_block_hash = sha256("이전 블록 hash");
	block_info.block_clock = xgetclock();
	block_info.ntx = txid_info.txidlist.size();

	string txhash = sha256(block_txlist);
	seriz_add(blockszr, block_info);
	string infohash = sha256(blockszr.getstring());
	string blockhash = sha256(infohash + txhash);

	memcpy(block_info.block_hash, blockhash.c_str(), blockhash.size());

	seriz_add(blockszr, block_info);

	string block_data = blockszr.data() + block_txlist;
	logprintf(1, "++++++++++블록 생성 완료: %ld 파일 출력 필요\n", block_data.size());
	printf("++++++++++블록 생성 완료: %ld 파일 출력 필요\n", block_data.size());

	// 다음 블록으로..
	_block_height = txid_info.block_height + 1;

	txid_info.txidlist.clear();
	txid_info.on_air = 0;
}
