//
// TX Serialize
//

#include "txcommon.h"


////////////////////////////////////////////////////////////////////////////////
// Serialize

int	seriz_add(xserialize& xsz, block_info_t& tx)
{
	int ret = 0;
	
	ret = xsz << tx.block_hash;		// 블록 해시 sha256(block_info_t + orgdataser 리스트)
						// 아래의 내용에 대한 hash 값임 (자신은 0000으로 초기화된 상태)
	ret = xsz << tx.block_height;		// 블록 번호 
	ret = xsz << tx.gen_addr;		// 블록 생성자
	ret = xsz << tx.signature;		// sign(orgdataser 리스트)
	ret = xsz << tx.prev_block_hash;	// 이전 블록 hash
	ret = xsz << tx.block_clock;		// 블록 생성 시각 
	ret = xsz << tx.ntx;			// TX 개수 

	return ret;
}


int	seriz_add(xserialize& xsz, tx_block_gen_req_t& tx)
{
	int ret = xsz << tx.txid;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_block_gen_reply_t& tx)
{
	int ret = 0;
	
	ret = xsz << tx.ntotal;
	ret = xsz << tx.nfail;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_block_gen_t& tx)
{
	int ret = 0;
	
	ret = xsz << tx.sign_hash;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_header_t& tx)
{
	int ret = 0;

	ret = xsz << tx.nodeid;		// node id
	ret = xsz << tx.type;		// TX_xxx
	ret = xsz << tx.data_length;	// sign data length
	ret = xsz << tx.signature;	// signature of data
	ret = xsz << tx.from_addr;	// 데이터 부분에 from_addr가 없는 경우에 사용
	ret = xsz << tx.txclock;	// tx generation clock

	ret = xsz << tx.block_height;	// 블록 번호
	ret = xsz << tx.status;		// STAT_VERIFY_xx
	ret = xsz << tx.valid;		// 0=invalid 1=valid -1=none
	ret = xsz << tx.txid;		// transaction id: sha256(sign)
	ret = xsz << tx.flag;		// FLAG_xxx

	return ret;
}


int	seriz_add(xserialize& xsz, tx_verify_reply_t& tx)
{
	int ret = xsz << tx.txid;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_create_token_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.to_addr;
	ret = xsz << tx.token_name;     // 256 bytes

	ret = xsz << tx.quantity;       // 발행 수량
	ret = xsz << tx.smallest_unit;  // 최소 단위
	ret = xsz << tx.native_amount;  // native 수량

	ret = xsz << tx.access;         // "1"=1time / "##"=N번 추가 생성 가능 / "forever"=계속 생성 가능
	ret = xsz << tx.start_time;     // token 시작 시간 (0이면 즉시)
	ret = xsz << tx.expire_time;    // token 중단 시간 (0이면 계속)

	ret = xsz << tx.user_data;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_send_token_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.to_addr;
	ret = xsz << tx.token_name;
	ret = xsz << tx.amount;
	ret = xsz << tx.native_amount;
	ret = xsz << tx.fee;

	ret = xsz << tx.user_data;

	return ret;
}


////////////////////////////////////////////////////////////////////////////////
// De-serialize

int	deseriz(xserialize& xsz, block_info_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> (char **)&tx.block_hash;	// 블록 해시 sha256(block_info_t + orgdataser 리스트)
						// 아래의 내용에 대한 hash 값임 (자신은 0000으로 초기화된 상태)
	ret = xsz >> tx.block_height;		// 블록 번호 
	ret = xsz >> tx.gen_addr;		// 블록 생성자
	ret = xsz >> tx.signature;		// sign(orgdataser 리스트)
	ret = xsz >> tx.prev_block_hash;	// 이전 블록 hash
	ret = xsz >> tx.block_clock;		// 블록 생성 시각 
	ret = xsz >> tx.ntx;			// TX 개수 

	if (dump)
	{
		printf("    block_hash = %s\n", tx.block_hash);
		printf("    block_height = %lu\n", tx.block_height);
		printf("    gen_addr = %s\n", tx.gen_addr.c_str());
		printf("    signature = %s\n", tx.signature.c_str());
		printf("    prev_block_hash = %s\n", tx.prev_block_hash.c_str());
		printf("    block_clock = %.3f\n", tx.block_clock);
		printf("    ntx = %d\n", tx.ntx);
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_block_gen_req_t& tx, int dump)
{
	int ret = xsz >> tx.txid;

	if (dump)
	{
		printf("    txid = %s\n", tx.txid.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_block_gen_reply_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.ntotal;
	ret = xsz >> tx.nfail;

	if (dump)
	{
		printf("    ntotal = %d\n", tx.ntotal);
		printf("    nfail = %d\n", tx.nfail);
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_block_gen_t& tx, int dump)
{
	int ret = xsz >> tx.sign_hash;

	if (dump)
	{
		printf("    sign_hash = %s\n", tx.sign_hash.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_header_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.nodeid;		// node id
	ret = xsz >> tx.type;		// TX_xxx
	ret = xsz >> tx.data_length;	// sign data length
	ret = xsz >> tx.signature;	// signature of data
	ret = xsz >> tx.from_addr;	// 데이터 부분에 from_addr가 없는 경우에 사용
	ret = xsz >> tx.txclock;	// tx generation clock

	ret = xsz >> tx.block_height;	// 블록 번호
	ret = xsz >> tx.status;		// STAT_VERIFY_xx
	ret = xsz >> tx.valid;		// 0=invalid 1=valid -1=none
	ret = xsz >> tx.txid;		// transaction id: sha256(sign)
	ret = xsz >> tx.flag;		// FLAG_xxx

	if (dump)
	{
		printf("    hdr::nodeid = %u\n", tx.nodeid);
		printf("    hdr::type = %s (%u == 0x%08X)\n", 
			get_type_name(tx.type), tx.type, tx.type);
		printf("    hdr::data length = %ld\n", tx.data_length);
		printf("    hdr::signature = %s (%ld)\n", tx.signature.c_str(), tx.signature.size());
		printf("    hdr::from_addr = %s (%ld)\n", tx.from_addr.c_str(), tx.from_addr.size());
		printf("    hdr::txclock = %.3f\n", tx.txclock);

		printf("    hdr::block_height = %lu\n", tx.block_height);
		printf("    hdr::status = %s (%u == 0x%08X)\n", 
			get_status_name(tx.status), tx.status, tx.status);
		printf("    hdr::valid = %d\n", tx.valid);
		printf("    hdr::txid = %s (%ld)\n", tx.txid.c_str(), tx.txid.size());
		printf("    hdr::sent = 0x%08X\n", tx.flag);
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_verify_reply_t& tx, int dump)
{
	int ret = xsz >> tx.txid;

	if (dump)
	{
		printf("    txid = %s\n", tx.txid.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_create_token_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.to_addr;
	ret = xsz >> tx.token_name;     // 256 bytes

	ret = xsz >> tx.quantity;       // 발행 수량
	ret = xsz >> tx.smallest_unit;  // 최소 단위
	ret = xsz >> tx.native_amount;  // native 수량

	ret = xsz >> tx.access;         // "1"=1time / "##"=N번 추가 생성 가능 / "forever"=계속 생성 가능
	ret = xsz >> tx.start_time;     // token 시작 시간 (0이면 즉시)
	ret = xsz >> tx.expire_time;    // token 중단 시간 (0이면 계속)

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    to_addr = %s\n", tx.to_addr.c_str());
		printf("    token_name = %s\n", tx.token_name.c_str());

		printf("    quantity = %lu\n", tx.quantity);
		printf("    smallest_unit = %lu\n", tx.smallest_unit);
		printf("    native_amount = %lu\n", tx.native_amount);

		printf("    access = %s\n", tx.access.c_str());
		printf("    start_time = %ld\n", tx.start_time);
		printf("    expire_time = %ld\n", tx.expire_time);

		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_send_token_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.to_addr;
	ret = xsz >> tx.token_name;
	ret = xsz >> tx.amount;
	ret = xsz >> tx.native_amount;
	ret = xsz >> tx.fee;
	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    to_addr = %s\n", tx.to_addr.c_str());
		printf("    token_name = %s\n", tx.token_name.c_str());
		printf("    amount = %.6f\n", tx.amount);
		printf("    native_amount = %.6f\n", tx.native_amount);
		printf("    fee = %.6f\n", tx.fee);
		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


