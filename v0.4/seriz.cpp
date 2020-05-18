//
// TX Serialize
//

#include "txcommon.h"


////////////////////////////////////////////////////////////////////////////////
// Serialize

int	seriz_add(xserialize& xsz, tx_block_txid_t& tx)
{
	int ret = xsz << tx.txid;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_block_sync_reply_t& tx)
{
	int ret = 0;
	
	ret = xsz << tx.ntotal;
	ret = xsz << tx.nfail;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_header_t& tx)
{
	int ret = 0;

	ret = xsz << tx.nodeid;		// node id
	ret = xsz << tx.type;		// TX_xxx
	ret = xsz << tx.status;		// STAT_VERIFY_xx
	ret = xsz << tx.data_length;	// sign data length
	ret = xsz << tx.valid;		// 0=invalid 1=valid -1=none
	ret = xsz << tx.signature;	// signature of data
	ret = xsz << tx.txid;		// transaction id: sha256(sign)
	ret = xsz << tx.from_addr;	// 데이터 부분에 from_addr가 없는 경우에 사용
	ret = xsz << tx.txclock;	// tx generation clock
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
        ret = xsz << tx.sign_clock;

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
	ret = xsz << tx.sign_clock;

	return ret;
}


////////////////////////////////////////////////////////////////////////////////
// De-serialize

int	deseriz(xserialize& xsz, tx_block_txid_t& tx, int dump)
{
	int ret = xsz >> tx.txid;

	if (dump)
	{
		printf("    txid = %s\n", tx.txid.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_block_sync_reply_t& tx, int dump)
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


int	deseriz(xserialize& xsz, tx_header_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.nodeid;		// node id
	ret = xsz >> tx.type;		// TX_xxx
	ret = xsz >> tx.status;		// STAT_VERIFY_xx
	ret = xsz >> tx.data_length;	// sign data length
	ret = xsz >> tx.valid;		// 0=invalid 1=valid -1=none
	ret = xsz >> tx.signature;	// signature of data
	ret = xsz >> tx.txid;		// transaction id: sha256(sign)
	ret = xsz >> tx.from_addr;	// 데이터 부분에 from_addr가 없는 경우에 사용
	ret = xsz >> tx.txclock;	// tx generation clock
	ret = xsz >> tx.flag;		// FLAG_xxx

	if (dump)
	{
		printf("    hdr::nodeid = %u\n", tx.nodeid);
		printf("    hdr::type = %s (%u == 0x%08X)\n", 
			get_type_name(tx.type), tx.type, tx.type);
		printf("    hdr::status = %s (%u == 0x%08X)\n", 
			get_status_name(tx.status), tx.status, tx.status);
		printf("    hdr::data length = %ld\n", tx.data_length);
		printf("    hdr::valid = %d\n", tx.valid);
		printf("    hdr::sign = %s\n", tx.signature.c_str());
		printf("    hdr::txid = %s\n", tx.txid.c_str());
		printf("    hdr::from_addr = %s\n", tx.from_addr.c_str());
		printf("    hdr::txclock = %.3f\n", tx.txclock);
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
        ret = xsz >> tx.sign_clock;

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
		printf("    sign_clock = %.3f\n", tx.sign_clock);
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
	ret = xsz >> tx.sign_clock;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    to_addr = %s\n", tx.to_addr.c_str());
		printf("    token_name = %s\n", tx.token_name.c_str());
		printf("    amount = %.6f\n", tx.amount);
		printf("    native_amount = %.6f\n", tx.native_amount);
		printf("    fee = %.6f\n", tx.fee);
		printf("    user_data = %s\n", tx.user_data.c_str());
		printf("    sign_clock = %.3f\n", tx.sign_clock);
	}

	return ret;
}


