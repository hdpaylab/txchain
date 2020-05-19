#include "txcommon.h"


map<uint32_t, int>	_block_gen_node;	// key=nodeid value=nfail


int	block_gen(txdata_t& txdata);


//
// 일정 주기마다 블록 생성 요청을 보내서 합의를 받은 다음 블록 생성함 
//
void	*thread_block_gen(void *info_p)
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

		int ntx = block_gen(txdata);
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
int	block_gen(txdata_t& txdata)
{
	vector<string> txidlist;
	int ntxidlist = 0;

	txidlist.resize(100000);

	for (int nn = 0; nn < (ssize_t)_mempool_count; nn++)
	{
		txdata_t txdata = _mempool[nn];

		if (txdata.hdr.flag & FLAG_TX_LOCK)	// 다른 노드에서 블록 생성하기 위해 TX 잠근 경우 
			continue;
		if (!(txdata.hdr.flag & FLAG_SENT_TX))	// 아직 동기화를 위해서 발송하지 않은 경우 
			continue;

		printf("BLOCK: add %d:%s\n", ntxidlist, txdata.hdr.txid.c_str());

		txdata.hdr.flag |= FLAG_TX_LOCK;	// TX LOCK
		txidlist[ntxidlist] = txdata.hdr.txid;
		ntxidlist++;
		if (ntxidlist >= 100000)
			break;
	}

	if (ntxidlist <= 0)
		return ntxidlist;

	txidlist.resize(ntxidlist);
	txidlist.shrink_to_fit();


	xserialize hdrszr, bodyszr;
	tx_header_t hdr;

	// 바디 serialization
	for (int nn = 0; nn < ntxidlist; nn++)
	{
		tx_block_gen_req_t	block_txid;

		block_txid.txid = txidlist[nn];
		seriz_add(bodyszr, block_txid);
	}

	// 헤더 serialization
	hdr.nodeid = getpid();	// 임시로 
	hdr.type = TX_BLOCK_GEN_REQ;
	hdr.status = 0;
	hdr.data_length = bodyszr.size();
	hdr.valid = -1;
	hdr.txid = string();
	hdr.from_addr = from_addr;
	hdr.txclock = xgetclock();
	hdr.flag = 0;
	hdr.signature = sign_message_bin(privkey, bodyszr.data(), bodyszr.size(), 
				&_params.PrivHelper, &_params.AddrHelper);

	seriz_add(hdrszr, hdr);

	printf("    block sync: signature: %s\n", hdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(from_addr, hdr.signature.c_str(), 
				bodyszr.data(), bodyszr.size(), &_params.AddrHelper);
	printf("    BLOCK_GEN: verify_check=%d\n", verify_check);
	printf("\n");

	txdata.hdrser = hdrszr.getstring();
	txdata.bodyser = bodyszr.getstring();

	_sendq.push(txdata);

	return ntxidlist;
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
				txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
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
				&_params.PrivHelper, &_params.AddrHelper);
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
				txdata.bodyser.c_str(), hp->data_length, &_params.AddrHelper);
	if (hp->valid)
	{
		hp->txid = hp->valid ? sha256(hp->signature) : "ERROR: Transaction verification failed!";

		_block_gen_node[hp->nodeid] = reply.nfail;

		printf("    TX_BLOCK_GEN_REPLY verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());
		printf("    ntotal=%d nfail=%d\n", reply.ntotal, reply.nfail);
	}
	else
	{
		printf("    TX_BLOCK_GEN_REPLY verify FAILED=%d \n", hp->valid);
	}
}
