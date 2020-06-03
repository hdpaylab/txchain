////////////////////////////////////////////////////////////////////////////////
//
// TX size: 80 bytes
//
////////////////////////////////////////////////////////////////////////////////

#include "txcommon.h"


block_txid_info_t	_self_txid_info;	// 자체적으로 블록 생성을 위한 txidlist
block_txid_info_t	_recv_txid_info;	// 블록 생성을 위해서 다른 노드에서 보낸 txidlist

block_info_t		_genesis_block_hdr;	// Genesis block header
block_info_t		_last_block_hdr;	// latest block header
uint32_t		_last_block_file_no = 0;	// 블록 파일 번호 

map<uint32_t, int>	_txid_reply_node;	// key=nodeid value=nfail

vector<string>	_next_block_txids;		// txid list of next block


size_t	block_gen_prepare();
void	send_block_gen_reply(txdata_t& txdata);	// BLOCK 생성 명령 발송 
int	make_block(block_info_t& block_hdr, string& block_data);


//
// 일정 주기마다 블록 생성 요청을 보내서 합의를 받은 다음 블록 생성함 
//
void	*thread_txid_info(void *info_p)
{
	int	chainport = *(int *)info_p;
	double	blocktime = xgetclock();


	logprintf(3, "thread_txid_info(): chainport=%d\n", chainport);

	_self_txid_info.on_air = 0;
	_self_txid_info.block_height = _last_block_hdr.block_height + 1;
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

		logprintf(3, "    prepare BLOCK: prepare %d diff=%.3f flag=0x%08X %s\n", 
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

		logprintf(3, "    prepare BLOCK Serialize: txid=%s \n", txid.c_str());
	}

	_mempool_lock.unlock();

	// 헤더 serialization
	hdr.nodeid = getpid();	// 임시로 
	hdr.block_height = _last_block_hdr.block_height + 1;	// 다음 생성할 블록 번호 
	hdr.type = TX_BLOCK_GEN_REQ;
	hdr.data_length = bodyszr.size();
	hdr.from_addr = _keypair.walletAddr;
	hdr.txclock = xgetclock();
	hdr.signature = sign_message_bin(_keypair.privateKey.c_str(), bodyszr.data(), bodyszr.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	seriz_add(hdrszr, hdr);

	logprintf(1, "    BLOCK_PREPARE: signature: %s\n", hdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(_keypair.walletAddr.c_str(), hdr.signature.c_str(), 
				bodyszr.data(), bodyszr.size(), &_netparams.AddrHelper);
	logprintf(1, "    BLOCK_PREPARE: verify_check=%d\n", verify_check);

	newtxdata.hdr = hdr;
	newtxdata.hdrser = hdrszr.getstring();
	newtxdata.bodyser = bodyszr.getstring();

	// 발송 
	_sendq.push(newtxdata);

	_self_txid_info.block_height = _last_block_hdr.block_height + 1;
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
	hdr.from_addr = _keypair.walletAddr;
	hdr.txclock = xgetclock();
	hdr.signature = sign_message_bin(_keypair.privateKey.c_str(), newbodyszr.data(), newbodyszr.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	xserialize hdrszr;
	seriz_add(hdrszr, hdr);

	newtxdata.hdr = hdr;
	newtxdata.hdrser = hdrszr.getstring();
	newtxdata.bodyser = newbodyszr.getstring();

	logprintf(2, "    Add to sendq(TX_BLOCK_GEN_REPLY): type=%s nfail=%d\n",
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

	logprintf(2, "    TX_BLOCK_GEN_REPLY: fail=%d\n", hp->status);

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
			logprintf(3, "    MAP [%u] => %d\n", it->first, it->second);
			if (it->second <= 0)
				nok++;
		}

		logprintf(2, "    TX_BLOCK_GEN_REPLY verify result=%d txid=%s\n", hp->valid, hp->txid.c_str());
		logprintf(2, "    ntotal=%d nfail=%d\n", reply.ntotal, reply.nfail);

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
	tx_sign_hash_t cmd;
	xserialize hdrszr, bodyszr;

	// Body serialize
	cmd.sign_hash = _self_txid_info.sign_hash;

	seriz_add(bodyszr, cmd);

	// Header serialize
	hdr.nodeid = getpid();
	hdr.block_height = _last_block_hdr.block_height + 1;	// 다음 생성할 블록 번호 
	hdr.type = TX_BLOCK_GEN;
	hdr.data_length = bodyszr.size();
	hdr.from_addr = _keypair.walletAddr;
	hdr.txclock = xgetclock();
	hdr.signature = sign_message_bin(_keypair.privateKey.c_str(), bodyszr.data(), bodyszr.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	seriz_add(hdrszr, hdr);

	newtxdata.hdr = hdr;
	newtxdata.hdrser = hdrszr.getstring();
	newtxdata.bodyser = bodyszr.getstring();

	logprintf(2, "    Add to sendq(TX_BLOCK_GEN): type=%s sign_hash=%s\n",
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

	txid_info.block_height = _last_block_hdr.block_height + 1;

	logprintf(1, "BLOCK_GEN: mempool size=%ld height=%ld\n", 
		_mempoolmap.size(), txid_info.block_height);

	// 실제 블록 생성: 전체 TX 목록에 대해서 sign 후 저장 (이전 블록 hash 필요)
	_mempool_lock.lock();

	// FLAG_TX_LOCK 표시된 tx는 블록으로 저장될 내용임 
	int marktx = 0;
	for (ssize_t ntx = 0; ntx < (ssize_t)txid_info.txidlist.size(); ntx++)
	{
		string txid = txid_info.txidlist[ntx];
		txdata_t& curtxdata = _mempoolmap[txid];

		if (curtxdata.hdr.flag & FLAG_TX_LOCK)
		{
			file_tx_header_t fhdr;

			txdata_t newtxdata = curtxdata;

			parse_header_body(newtxdata);

			fhdr.nodeid = newtxdata.hdr.nodeid;
			fhdr.type = newtxdata.hdr.type;
			fhdr.data_length = newtxdata.hdr.data_length;
			fhdr.signature = newtxdata.hdr.signature;
			fhdr.txclock = newtxdata.hdr.txclock;
			fhdr.recvclock = newtxdata.hdr.recvclock;

			xserialize hdrszr;
			seriz_add(hdrszr, fhdr);

			block_txlist += hdrszr.getstring() + newtxdata.bodyser;	// 원본 serialized 데이터를 연결 

			curtxdata.hdr.flag |= FLAG_TX_DELETE;	// 삭제할 것

			marktx++;
			logprintf(2, "    Block [%ld] %d hdrsz=%ld bodysz=%ld value=%d\n", 
				txid_info.block_height, marktx, newtxdata.hdrser.size(), 
				newtxdata.bodyser.size(), newtxdata.hdr.value);
		}
		else
		{
			logprintf(5, "    Skipped: mempool [%ld] txid=%s value=%d flag=%08X\n", 
				marktx, curtxdata.hdr.txid.c_str(), curtxdata.hdr.value, curtxdata.hdr.flag);
		}
	}

	// mempool에서 저장된 내용 삭제 
	int ndel = 0;
	for (ssize_t ntx = 0; ntx < (ssize_t)txid_info.txidlist.size(); ntx++)
	{
		string txid = txid_info.txidlist[ntx];
		txdata_t& curtxdata = _mempoolmap[txid];

		if (curtxdata.hdr.flag & FLAG_TX_DELETE)
		{
			logprintf(2, "    Mempool %ld 삭제: txid=%s\n", ntx, curtxdata.hdr.txid.c_str());

			_mempoolmap.erase(txid);	// mempoolmap에서 제거 
			ndel++;
		}
	}

	_mempool_lock.unlock();

	logprintf(1, "    BLOCK_GEN: txtotal=%ld / marktx=%d ndel=%d \n",
		txid_info.txidlist.size(), marktx, ndel);

	string txhash = sha256(block_txlist);

	// 블록 정보 setup
	block_info_t block_hdr;

	block_hdr.block_height = _last_block_hdr.block_height + 1;
	block_hdr.block_gen_addr = _keypair.walletAddr;
	block_hdr.block_signature = sign_message_bin(_keypair.privateKey.c_str(), block_txlist.c_str(), block_txlist.size(), 
				&_netparams.PrivHelper, &_netparams.AddrHelper);
	block_hdr.prev_block_hash = _last_block_hdr.block_hash;
	block_hdr.block_clock = xgetclock();
	block_hdr.block_numtx = txid_info.txidlist.size();

	xserialize hdrszr;
	seriz_add(hdrszr, block_hdr);		// block_hash == null && block_size == 0

	string infohash = sha256(hdrszr.getstring());
	block_hdr.block_hash = sha256(infohash + txhash);

	hdrszr.clear();
	seriz_add(hdrszr, block_hdr);	// block_hash == "HASH" && block_size > 0

	block_hdr.block_size = hdrszr.size() + block_txlist.size();	// 전체 블록 크기 다시 계산

	hdrszr.clear();
	seriz_add(hdrszr, block_hdr);	// block_hash == null && block_size == 0

	string block_data = hdrszr.getstring() + block_txlist;

	make_block(block_hdr, block_data);

	// 다음 블록으로..
	_last_block_hdr = block_hdr;

	txid_info.txidlist.clear();
	txid_info.on_air = 0;
}


//
// 블록 생성:
//	block_hdr: 현재 저장하려는 블록의 정보 
//	block_data: 헤더 부분과 TX 리스트가 serialize되어 있음 
//
int	make_block(block_info_t& block_hdr, string& block_data)
{
	char	path[256] = {0};

	snprintf(path, sizeof(path), "blocks/block-%06d-%d.dat", _last_block_file_no, _clientport);
	FILE	*bfp = fopen(path, "r+b");
	if (bfp)
	{
		fseek(bfp, 0L, SEEK_END);
		size_t wbytes = fwrite(block_data.c_str(), 1, block_data.size(), bfp);
		fflush(bfp);
		fclose(bfp);

		if (wbytes != block_data.size())
		{
			perror("fwrite");
			logprintf(0, "ERROR: Block %ld write failed to '%s'!\n", block_hdr.block_height, path);
			return -1;
		}
		logprintf(1, "++++++++++블록 %ld 생성 완료: size=%ld \n", block_hdr.block_height, block_data.size());
	}
	else
	{
		logprintf(0, "ERROR: Cannot open block db file '%s'!\n", path);
		return -1;
	}
	return 0;
}


//
//
//
int	check_blocks()
{
	char	path[256] = {0};
	uint64_t nblock = 0;

	printf("Checking blocks...\n");

	snprintf(path, sizeof(path), "blocks/block-%06d-%d.dat", _last_block_file_no, _clientport);
	FILE	*bfp = fopen(path, "rb");
	if (bfp == NULL)
	{
		logprintf(0, "ERROR: Cannot open block db file '%s'!\n", path);
		return -1;
	}

	// Genesis block skip
	fseek(bfp, GENESIS_BLOCK_SIZE, SEEK_SET);

	for (nblock = 1; ; nblock++)
	{
		long	start_pos = ftell(bfp);
		printf("Read block %ld: offset=%ld\n", nblock, start_pos);

		char	sizebuf[sizeof(size_t) + 1] = {0};	// +1은 serialize type 1바이트 추가 
		size_t rbytes = fread(sizebuf, 1, sizeof(size_t) + 1, bfp);
		if (rbytes != sizeof(size_t) + 1)
			break;

		size_t block_size = *(size_t *)&sizebuf[1];
		printf("block size=%ld \n", block_size);

		fseek(bfp, start_pos, SEEK_SET);
		char	*blockbuf = (char *) calloc(1, block_size);
		if (blockbuf == NULL)
		{
			logprintf(0, "WARNING: calloc(%ld) failed at check_block!\n", block_size);
			continue;
		}

		rbytes = fread(blockbuf, 1, block_size, bfp);
		if (rbytes != block_size)
		{
			logprintf(0, "WARNING: Block %d read failed!\n", _last_block_hdr.block_height + 1);
			break;
		}

		xserialize szr;
		block_info_t block_hdr;

		szr.setdata(blockbuf, rbytes);
		deseriz(szr, block_hdr);

		printf("Loading BLOCK %ld: %ld bytes\n", block_hdr.block_height, block_size);
		printf("	block_size	= %lu\n", block_hdr.block_size);
		printf("	block_hash	= %s\n", block_hdr.block_hash.c_str()); 
		printf("	block_height	= %lu\n", block_hdr.block_height);
		printf("	block_version	= 0x%08lX\n", block_hdr.block_version);
		printf("	prev_block_hash	= %s\n", block_hdr.prev_block_hash.c_str()); 
		printf("	block_clock	= %.3f\n", block_hdr.block_clock);
		printf("	block_numtx	= %lu\n", block_hdr.block_numtx);
		printf("	block_gen_addr	= %s\n", block_hdr.block_gen_addr.c_str()); 
		printf("	block_signature	= %s\n", block_hdr.block_signature.c_str()); 
		printf("\n");

		// TX list dump
		for (ssize_t ntx = 0; ntx < (ssize_t)block_hdr.block_numtx; ntx++)
		{
			file_tx_header_t fhdr;
			txid_info_req_t tx;

			deseriz(szr, fhdr);

			printf("BLOCK %ld TX %ld:\n", block_hdr.block_height, ntx);
			printf("    nodeid	= %u\n", fhdr.nodeid);
			printf("    type	= %s (%u == 0x%08X)\n", get_type_name(fhdr.type), fhdr.type, fhdr.type);
			printf("    data length	= %ld\n", fhdr.data_length);
			printf("    signature	= %s (%ld)\n", fhdr.signature.c_str(), fhdr.signature.size());
			printf("    txclock	= %.3f\n", fhdr.txclock);
			printf("    recvclock	= %.3f\n", fhdr.recvclock);

			if (fhdr.type == TX_SEND_TOKEN)
			{
				tx_send_token_t tx;

				deseriz(szr, tx);

				printf("    tx type: SEND_TOKEN:\n");
				printf("	from_addr	= %s\n", tx.from_addr.c_str());
				printf("	to_addr		= %s\n", tx.to_addr.c_str());
				printf("	token_name	= %s\n", tx.token_name.c_str());
				printf("	amount		= %.6f\n", tx.amount);
				printf("	native_amount	= %.6f\n", tx.native_amount);
				printf("	fee		= %.6f\n", tx.fee);
				printf("	user_data	= %s\n", tx.user_data.c_str());
			}
			printf("\n");

			sleepms(10);
		}
		printf("\n");
	}
	printf("\n");

	return 0;
}


//
// Genesis block 최초 생성
//
int	make_genesis_block(const char *path)
{
	char	*block0 = (char *)calloc(1, GENESIS_BLOCK_SIZE);
	char	*bp = block0;

	assert(block0 != NULL);

	// 랜덤 값으로 채움 
	for (int idx = 0; idx < GENESIS_BLOCK_SIZE; idx++)
	{
		block0[idx] = rand() % 0x00FF;
	}

	_genesis_block_hdr.block_size = GENESIS_BLOCK_SIZE;	// include 8 bytes block_size
	_genesis_block_hdr.block_height = 0;
	_genesis_block_hdr.block_version = 0x00000004;
	_genesis_block_hdr.block_clock = xgetclock();
	_genesis_block_hdr.block_numtx = 0;
	_genesis_block_hdr.block_gen_addr = _keypair.walletAddr;
	// 서명은 address에 대해서 수행함 
	_genesis_block_hdr.block_signature = sign_message_bin(_keypair.privateKey.c_str(), 
						_keypair.walletAddr.c_str(), _keypair.walletAddr.size(),
						&_netparams.PrivHelper, &_netparams.AddrHelper);

	printf("Writing genesis block:\n");
	printf("	block_size	= %lu\n", _genesis_block_hdr.block_size);
	printf("	block_hash	= %s\n", _genesis_block_hdr.block_hash.c_str()); 
	printf("	block_height	= %lu\n", _genesis_block_hdr.block_height);
	printf("	block_version	= 0x%08lX\n", _genesis_block_hdr.block_version);
	printf("	prev_block_hash	= %s\n", _genesis_block_hdr.prev_block_hash.c_str()); 
	printf("	block_clock	= %.3f\n", _genesis_block_hdr.block_clock);
	printf("	block_numtx	= %lu\n", _genesis_block_hdr.block_numtx);
	printf("	block_gen_addr	= %s\n", _genesis_block_hdr.block_gen_addr.c_str()); 
	printf("	block_signature	= %s\n", _genesis_block_hdr.block_signature.c_str()); 
	printf("\n");

	// 헤더를 serialization 
	xserialize szr;
	seriz_add(szr, _genesis_block_hdr);

	memcpy(bp, szr.getstring().c_str(), szr.size());	// 헤더 디스크 추가 
	bp += 518;		// 5.18 ^^

	// 암호 패스워드 생성 
	char	passwd[32] = {0};
	for (int ii = 0; ii < 32; ii++)
	{
		srand((uint32_t)(xgetclock() * 1000000));	// micro-second srand()
		passwd[ii] = rand() % 0x00FF;
		usleep(rand() % 100 + 1);
	}
	printf("Password: "); dumpbin((const char *)passwd, 32);

	memcpy(bp, passwd, 32);			// 패스워드 디스크 추가 
	bp += 64;

	passwd[6] = 6;				// 패스워드 변화..
	passwd[1] = 1;
	passwd[2] = 2;
	passwd[0] = 0;

	string basepass((const char *)passwd, 32);
	string enc_passwd = sha256(basepass, false);	// 실제 암호화 패스워드 
	
	printf("Generating genesis block:\n");
	printf("	privkey		= %s\n", _keypair.privateKey.c_str());
	printf("	pubkey		= %s\n", _keypair.pubkey.c_str());
	printf("	pubkeyHash	= %s\n", _keypair.pubkeyHash.c_str());
	printf("	walletAddr	= %s\n", _keypair.walletAddr.c_str());
	printf("\n");

	// masterkey의 hash값 구함 
	string mkey((const char *)_keypair.secret.begin(), 32);
	printf("Before AES256 encrypt: "); dumpbin((const char *)mkey.c_str(), 32);
	string mkey_hash = sha256(mkey, false);

	// masterkey 암호화 
	string enc_mkey = aes256_encrypt(enc_passwd, mkey);
	printf("After AES256 encrypt: "); dumpbin((const char *)enc_mkey.c_str(), 32);

	memcpy(bp, enc_mkey.c_str(), 32);	// 암호화된 masterkey 디스크 추가 
	bp += 64;
	
	memcpy(bp, mkey_hash.c_str(), 32);	// masterkey hash 디스크 추가 
	bp += 64;

	// block0에 기록 
	FILE *bfp = fopen(path, "wb");
	if (bfp)
	{
		size_t wbytes = fwrite(block0, 1, GENESIS_BLOCK_SIZE, bfp);
		fflush(bfp);
		fclose(bfp);

		if (wbytes != GENESIS_BLOCK_SIZE)
		{
			perror("fwrite");
			logprintf(0, "ERROR: Genesis block creation failed!\n");
			return -1;
		}
	}

	_last_block_hdr = _genesis_block_hdr;

	return 0;
}


//
// Genesis block loading: keypair 복구 (masterkey)
//
keypair_t load_genesis_block(const char *path)
{
	char	*block0 = (char *)calloc(1, GENESIS_BLOCK_SIZE);
	char	*bp = block0;

	assert(block0 != NULL);

	// block0 읽기 
	FILE *bfp = fopen(path, "rb");
	if (bfp)
	{
		size_t rbytes = fread(block0, 1, GENESIS_BLOCK_SIZE, bfp);
		fclose(bfp);

		if (rbytes != GENESIS_BLOCK_SIZE)
		{
			perror("fread");
			logprintf(0, "ERROR: Genesis block read failed! read=%ld bytes\n", rbytes);
			exit(-1);
		}
	}

	xserialize szr;

	string hdrstr(bp, 518);		// 헤더 부분 deserialize
	szr.setstring(hdrstr);
	deseriz(szr, _genesis_block_hdr);
	bp += 518;			// 5.18 ^^

	printf("Loading genesis block:\n");
	printf("	block_size	= %lu\n", _genesis_block_hdr.block_size);
	printf("	block_hash	= %s\n", _genesis_block_hdr.block_hash.c_str()); 
	printf("	block_height	= %lu\n", _genesis_block_hdr.block_height);
	printf("	block_version	= 0x%08lX\n", _genesis_block_hdr.block_version);
	printf("	prev_block_hash	= %s\n", _genesis_block_hdr.prev_block_hash.c_str()); 
	printf("	block_clock	= %.3f\n", _genesis_block_hdr.block_clock);
	printf("	block_numtx	= %lu\n", _genesis_block_hdr.block_numtx);
	printf("	block_gen_addr	= %s\n", _genesis_block_hdr.block_gen_addr.c_str()); 
	printf("	block_signature	= %s\n", _genesis_block_hdr.block_signature.c_str()); 

	uchar	passwd[32] = {0};
	memcpy(passwd, bp, 32);		// masterkey 암호 입수 
	bp += 64;
	printf("Password: "); dumpbin((const char *)passwd, 32);

	passwd[6] = 6;			// 패스워드 변화..
	passwd[1] = 1;
	passwd[2] = 2;
	passwd[0] = 0;

	string basepass((const char *)passwd, 32);
	string dec_passwd = sha256(basepass, false);    // 실제 암호화 패스워드

	uchar	enc_mkey[100] = {0};
	memcpy(enc_mkey, bp, 32);	// 암호화된 masterkey 입수 
	bp += 64;
	printf("Before AES256 decrypt: "); dumpbin((const char *)enc_mkey, 32);

	char	disk_mkey_hash[64] = {0};
	memcpy(disk_mkey_hash, bp, 32);	// masterkey hash 입수 
	bp += 64;

	// masterkey 복호화 
	string tmp_mkey((const char *)enc_mkey, 32);
	string mkey = aes256_decrypt(dec_passwd, tmp_mkey);
	printf("After AES256 decrypt: "); dumpbin((const char *)mkey.c_str(), 32);

	// 키페어 복구
	printf("\nLoading keypairs:\n");
	keypair_t keypair = create_keypair((const uchar *)mkey.c_str(), 32);

	// 마스터키 hash 구해서 디스크의 hash 값과 일치하는지 비교 
	string mkey_hash = sha256(mkey, false);

	// genesis 블록 생성시 만든 masterkey의 hash와 master key를 hash한 값이 같아야 master key가 동일하다는 검증이 됨
	if (memcmp(disk_mkey_hash, mkey_hash.c_str(), 32) != 0)
	{
		printf("ERROR: Genesis block private key verification failed!\n");
		printf("\n");
		exit(-1);
	}

	// 복구된 키페어의 정보와 헤더의 주소가 일치해야 함 
	assert(keypair.walletAddr == _genesis_block_hdr.block_gen_addr);

	int verify_check = verify_message_bin(_genesis_block_hdr.block_gen_addr.c_str(), 
				_genesis_block_hdr.block_signature.c_str(), 
				_genesis_block_hdr.block_gen_addr.c_str(), 
				_genesis_block_hdr.block_gen_addr.size(), &_netparams.AddrHelper);
	assert(verify_check == 1);

	printf("Genesis block private key and signature verified.\n");
	printf("\n");

	return keypair;
}
