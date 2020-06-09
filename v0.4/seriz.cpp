//
// TX Serialize
//

#include "txcommon.h"


////////////////////////////////////////////////////////////////////////////////
// Serialize

int	seriz_add(xserialize& xsz, block_info_t& tx)
{
	int ret = 0;
	
	ret = xsz << tx.block_size;		// 블록 크기 (자신을 제외한 아래 내용)
	ret = xsz << tx.block_hash;		// 블록 해시 sha256(block_info_t + orgdataser 리스트)
						// 아래의 내용에 대한 hash 값임 (자신은 0000으로 초기화된 상태)
	ret = xsz << tx.block_height;		// 블록 번호 
	ret = xsz << tx.block_version;		// 블록 버전 
	ret = xsz << tx.prev_block_hash;	// 이전 블록 hash
	ret = xsz << tx.block_clock;		// 블록 생성 시각 
	ret = xsz << tx.block_numtx;		// TX 개수 
	ret = xsz << tx.block_gen_addr;		// 블록 생성자 주소 
	ret = xsz << tx.block_signature;	// sign

	return ret;
}


int	seriz_add(xserialize& xsz, txid_info_req_t& tx)
{
	int ret = xsz << tx.txid;

	return ret;
}


int	seriz_add(xserialize& xsz, txid_info_reply_t& tx)
{
	int ret = 0;
	
	ret = xsz << tx.ntotal;
	ret = xsz << tx.nfail;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_sign_hash_t& tx)
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
	ret = xsz << tx.recvclock;	// 수신 clock

	ret = xsz << tx.block_height;	// 블록 번호
	ret = xsz << tx.status;		// STAT_VERIFY_xx
	ret = xsz << tx.valid;		// 0=invalid 1=valid -1=none
	ret = xsz << tx.txid;		// transaction id: sha256(sign)
	ret = xsz << tx.flag;		// FLAG_xxx
	ret = xsz << tx.value;

	return ret;
}


int	seriz_add(xserialize& xsz, file_tx_header_t& tx)
{
	int ret = 0;

	ret = xsz << tx.nodeid;		// node id
	ret = xsz << tx.type;		// TX_xxx
	ret = xsz << tx.data_length;	// sign data length
	ret = xsz << tx.signature;	// signature of data
	ret = xsz << tx.txclock;	// tx generation clock
	ret = xsz << tx.recvclock;	// 수신 clock

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
	ret = xsz << tx.fee;

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


int	seriz_add(xserialize& xsz, tx_create_channel_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.to_addr;
	ret = xsz << tx.channel_name;	// 256 bytes

	ret = xsz << tx.access;		// "local" "permission" "anyone read" "anyone read write"
	ret = xsz << tx.start_time;	// channel 시작 시간 (0이면 즉시)
	ret = xsz << tx.expire_time;	// channel 중단 시간 (0이면 계속)

	ret = xsz << tx.user_data;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_publish_channel_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.channel_name;	// 256 bytes
	ret = xsz << tx.key;		// 256 bytes
	ret = xsz << tx.value;		// size limit: max tx size

	ret = xsz << tx.user_data;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_create_contract_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.to_addr;
	ret = xsz << tx.contract_name;	// 256 bytes
	ret = xsz << tx.program;

	ret = xsz << tx.access;		// "local" "permission" "anyone read" "anyone read write"
	ret = xsz << tx.start_time;	// channel 시작 시간 (0이면 즉시)
	ret = xsz << tx.expire_time;	// channel 중단 시간 (0이면 계속)

	ret = xsz << tx.user_data;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_destroy_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.target_name;	// TOKEN / CHANNEL / CONTRACT
	ret = xsz << tx.action;		// "pause" "stop" "start" "destroy"

	ret = xsz << tx.user_data;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_grant_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.to_addr;
	ret = xsz << tx.isgrant;
	ret = xsz << tx.type_name;	// token, channel, contract, wallet, account
	ret = xsz << tx.permission;	// TOKEN: issue, admin
					// CHANNEL: admin, read, write,
					// CONTRACT: admin, read
					// WALLET: admin, read, send
					// ACCOUNT: admin, read

	ret = xsz << tx.start_time;	// permission 시작 시간 (0이면 즉시)
	ret = xsz << tx.expire_time;	// permission 중단 시간 (0이면 계속)

	ret = xsz << tx.user_data;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_create_wallet_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.to_addr;
	ret = xsz << tx.wallet_name;

	ret = xsz << tx.access;		// "local" "permission" "anyone read" "anyone read write"
	ret = xsz << tx.start_time;	// 시작 시간 (0이면 즉시)
	ret = xsz << tx.expire_time;	// 중단 시간 (0이면 계속)

	ret = xsz << tx.user_data;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_create_account_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.account_name;

	ret = xsz << tx.access;		// "local" "permission" "anyone read" "anyone read write"
	ret = xsz << tx.start_time;	// 시작 시간 (0이면 즉시)
	ret = xsz << tx.expire_time;	// 중단 시간 (0이면 계속)

	ret = xsz << tx.user_data;

	return ret;
}


int	seriz_add(xserialize& xsz, tx_control_t& tx)
{
	int ret = 0;

	ret = xsz << tx.from_addr;
	ret = xsz << tx.command;
	ret = xsz << tx.arg1;
	ret = xsz << tx.arg2;
	ret = xsz << tx.arg3;
	ret = xsz << tx.arg4;
	ret = xsz << tx.arg5;

	ret = xsz << tx.user_data;

	return ret;
}


int	seriz_add(xserialize& xsz, txstat_t& tx)
{
	int ret = 0;

	ret = xsz << tx.sender_id;	// seed=01
	ret = xsz << tx.timeout_clock;  // network timeout clock (default: 10 sec)
	ret = xsz << tx.recv_clock;	// receive clock
	ret = xsz << tx.send_clock;	// send clock for consensus
	ret = xsz << tx.reply_clock;	//
	ret = xsz << tx.reply_ok;	// map 필요
	ret = xsz << tx.reply_fail;	// map 필요

	return ret;
}


////////////////////////////////////////////////////////////////////////////////
// De-serialize

int	deseriz(xserialize& xsz, block_info_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.block_size;		// 블록 크기 (자신을 제외한 아래 내용)
	ret = xsz >> tx.block_hash;		// 블록 해시 sha256(block_info_t + orgdataser 리스트)
						// 아래의 내용에 대한 hash 값임 (자신은 0000으로 초기화된 상태)
	ret = xsz >> tx.block_height;		// 블록 번호 
	ret = xsz >> tx.block_version;		// 블록 버전 
	ret = xsz >> tx.prev_block_hash;	// 이전 블록 hash
	ret = xsz >> tx.block_clock;		// 블록 생성 시각 
	ret = xsz >> tx.block_numtx;		// TX 개수 
	ret = xsz >> tx.block_gen_addr;		// 블록 생성자 주소 
	ret = xsz >> tx.block_signature;	// sign

	if (dump)
	{
		printf("    block_size		= %lu\n", tx.block_size);
		printf("    block_hash		= %s\n", tx.block_hash.c_str()); 
		printf("    block_height	= %lu\n", tx.block_height);
		printf("    block_version	= 0x%08lX\n", tx.block_version);
		printf("    prev_block_hash	= %s\n", tx.prev_block_hash.c_str());
		printf("    block_clock		= %.3f\n", tx.block_clock);
		printf("    block_numtx		= %lu\n", tx.block_numtx);
		printf("    block_gen_addr	= %s\n", tx.block_gen_addr.c_str());
		printf("    block_signature	= %s\n", tx.block_signature.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, txid_info_req_t& tx, int dump)
{
	int ret = xsz >> tx.txid;

	if (dump)
	{
		printf("    txid = %s\n", tx.txid.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, txid_info_reply_t& tx, int dump)
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


int	deseriz(xserialize& xsz, tx_sign_hash_t& tx, int dump)
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
	ret = xsz >> tx.recvclock;	// 수신 clock

	ret = xsz >> tx.block_height;	// 블록 번호
	ret = xsz >> tx.status;		// STAT_VERIFY_xx
	ret = xsz >> tx.valid;		// 0=invalid 1=valid -1=none
	ret = xsz >> tx.txid;		// transaction id: sha256(sign)
	ret = xsz >> tx.flag;		// FLAG_xxx
	ret = xsz >> tx.value;

	if (dump)
	{
		printf("    hdr::nodeid = %u\n", tx.nodeid);
		printf("    hdr::type = %s (%u == 0x%08X)\n", 
			get_type_name(tx.type), tx.type, tx.type);
		printf("    hdr::data length = %ld\n", tx.data_length);
		printf("    hdr::signature = %s (%ld)\n", tx.signature.c_str(), tx.signature.size());
		printf("    hdr::from_addr = %s (%ld)\n", tx.from_addr.c_str(), tx.from_addr.size());
		printf("    hdr::txclock = %.3f\n", tx.txclock);
		printf("    hdr::recvclock = %.3f\n", tx.recvclock);

		printf("    hdr::block_height = %lu\n", tx.block_height);
		printf("    hdr::status = %s (%u == 0x%08X)\n", 
			get_status_name(tx.status), tx.status, tx.status);
		printf("    hdr::valid = %d\n", tx.valid);
		printf("    hdr::txid = %s (%ld)\n", tx.txid.c_str(), tx.txid.size());
		printf("    hdr::flag = 0x%08X\n", tx.flag);
		printf("    hdr::value = %d == 0x%08X\n", tx.value, tx.value);
	}

	return ret;
}


int	deseriz(xserialize& xsz, file_tx_header_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.nodeid;		// node id
	ret = xsz >> tx.type;		// TX_xxx
	ret = xsz >> tx.data_length;	// sign data length
	ret = xsz >> tx.signature;	// signature of data
	ret = xsz >> tx.txclock;	// tx generation clock
	ret = xsz >> tx.recvclock;	// 수신 clock

	if (dump)
	{
		printf("    hdr::nodeid = %u\n", tx.nodeid);
		printf("    hdr::type = %s (%u == 0x%08X)\n", get_type_name(tx.type), tx.type, tx.type);
		printf("    hdr::data length = %ld\n", tx.data_length);
		printf("    hdr::signature = %s (%ld)\n", tx.signature.c_str(), tx.signature.size());
		printf("    hdr::txclock = %.3f\n", tx.txclock);
		printf("    hdr::recvclock = %.3f\n", tx.recvclock);
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
	ret = xsz >> tx.fee;

	ret = xsz >> tx.access;         // "1"=1time / "##"=N번 추가 생성 가능 / "forever"=계속 생성 가능
	ret = xsz >> tx.start_time;     // token 시작 시간 (0이면 즉시)
	ret = xsz >> tx.expire_time;    // token 중단 시간 (0이면 계속)

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    to_addr = %s\n", tx.to_addr.c_str());
		printf("    token_name = %s\n", tx.token_name.c_str());

		printf("    quantity = %.15lg\n", tx.quantity);
		printf("    smallest_unit = %.15g\n", tx.smallest_unit);
		printf("    native_amount = %.15g\n", tx.native_amount);
		printf("    fee = %.15g\n", tx.fee);

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


int	deseriz(xserialize& xsz, tx_create_channel_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.to_addr;
	ret = xsz >> tx.channel_name;	// 256 bytes

	ret = xsz >> tx.access;		// "local" "permission" "anyone read" "anyone read write"
	ret = xsz >> tx.start_time;	// channel 시작 시간 (0이면 즉시)
	ret = xsz >> tx.expire_time;	// channel 중단 시간 (0이면 계속)

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    to_addr = %s\n", tx.to_addr.c_str());
		printf("    channel_name = %s\n", tx.channel_name.c_str());

		printf("    access = %s\n", tx.access.c_str());
		printf("    start_time = %ld\n", tx.start_time);
		printf("    expire_time = %ld\n", tx.expire_time);

		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_publish_channel_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.channel_name;	// 256 bytes
	ret = xsz >> tx.key;		// 256 bytes
	ret = xsz >> tx.value;		// size limit: max tx size

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    channel_name = %s\n", tx.channel_name.c_str());

		printf("    key = %s\n", tx.key.c_str());
		printf("    value = %s\n", tx.value.c_str());

		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_create_contract_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.to_addr;
	ret = xsz >> tx.contract_name;	// 256 bytes
	ret = xsz >> tx.program;

	ret = xsz >> tx.access;		// "local" "permission" "anyone read" "anyone read write"
	ret = xsz >> tx.start_time;	// channel 시작 시간 (0이면 즉시)
	ret = xsz >> tx.expire_time;	// channel 중단 시간 (0이면 계속)

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    to_addr = %s\n", tx.to_addr.c_str());
		printf("    contract_name = %s\n", tx.contract_name.c_str());
		printf("    program = %s\n", tx.program.c_str());

		printf("    access = %s\n", tx.access.c_str());
		printf("    start_time = %ld\n", tx.start_time);
		printf("    expire_time = %ld\n", tx.expire_time);

		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_destroy_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.target_name;	// TOKEN / CHANNEL / CONTRACT
	ret = xsz >> tx.action;		// "pause" "stop" "start" "destroy"

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    target_name = %s\n", tx.target_name.c_str());
		printf("    action = %s\n", tx.action.c_str());

		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_grant_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.to_addr;
	ret = xsz >> tx.isgrant;
	ret = xsz >> tx.type_name;	// token, channel, contract, wallet, account
	ret = xsz >> tx.permission;	// TOKEN: issue, admin
					// CHANNEL: admin, read, write,
					// CONTRACT: admin, read
					// WALLET: admin, read, send
					// ACCOUNT: admin, read

	ret = xsz >> tx.start_time;	// permission 시작 시간 (0이면 즉시)
	ret = xsz >> tx.expire_time;	// permission 중단 시간 (0이면 계속)

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    to_addr = %s\n", tx.to_addr.c_str());
		printf("    permission = %s\n", tx.permission.c_str());

		printf("    start_time = %ld\n", tx.start_time);
		printf("    expire_time = %ld\n", tx.expire_time);

		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_create_wallet_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.to_addr;
	ret = xsz >> tx.wallet_name;

	ret = xsz >> tx.access;		// "local" "permission" "anyone read" "anyone read write"
	ret = xsz >> tx.start_time;	// 시작 시간 (0이면 즉시)
	ret = xsz >> tx.expire_time;	// 중단 시간 (0이면 계속)

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    to_addr = %s\n", tx.to_addr.c_str());
		printf("    wallet_name = %s\n", tx.wallet_name.c_str());

		printf("    access = %s\n", tx.access.c_str());
		printf("    start_time = %ld\n", tx.start_time);
		printf("    expire_time = %ld\n", tx.expire_time);

		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_create_account_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.account_name;

	ret = xsz >> tx.access;		// "local" "permission" "anyone read" "anyone read write"
	ret = xsz >> tx.start_time;	// 시작 시간 (0이면 즉시)
	ret = xsz >> tx.expire_time;	// 중단 시간 (0이면 계속)

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    account_name = %s\n", tx.account_name.c_str());

		printf("    access = %s\n", tx.access.c_str());
		printf("    start_time = %ld\n", tx.start_time);
		printf("    expire_time = %ld\n", tx.expire_time);

		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, tx_control_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.from_addr;
	ret = xsz >> tx.command;
	ret = xsz >> tx.arg1;
	ret = xsz >> tx.arg2;
	ret = xsz >> tx.arg3;
	ret = xsz >> tx.arg4;
	ret = xsz >> tx.arg5;

	ret = xsz >> tx.user_data;

	if (dump)
	{
		printf("    from_addr = %s\n", tx.from_addr.c_str());
		printf("    command = %s\n", tx.command.c_str());
		printf("    arg1 = %s\n", tx.arg1.c_str());
		printf("    arg2 = %s\n", tx.arg2.c_str());
		printf("    arg3 = %s\n", tx.arg3.c_str());
		printf("    arg4 = %s\n", tx.arg4.c_str());
		printf("    arg5 = %s\n", tx.arg5.c_str());

		printf("    user_data = %s\n", tx.user_data.c_str());
	}

	return ret;
}


int	deseriz(xserialize& xsz, txstat_t& tx, int dump)
{
	int ret = 0;

	ret = xsz >> tx.sender_id;	// seed=01
	ret = xsz >> tx.timeout_clock;  // network timeout clock (default: 10 sec)
	ret = xsz >> tx.recv_clock;	// receive clock
	ret = xsz >> tx.send_clock;	// send clock for consensus

	ret = xsz >> tx.reply_clock;	//
	ret = xsz >> tx.reply_ok;	// map 필요
	ret = xsz >> tx.reply_fail;	// map 필요

	if (dump)
	{
		printf("    sender_id = %u\n", tx.sender_id);
		printf("    timeout_clock = %.3f\n", tx.timeout_clock);
		printf("    recv_clock = %.3f\n", tx.recv_clock);
		printf("    send_clock = %.3f\n", tx.send_clock);
		printf("    reply_clock = %.3f\n", tx.reply_clock);
		printf("    reply_ok = %u\n", tx.reply_ok);
		printf("    reply_fail = %u\n", tx.reply_fail);
	}

	return ret;
}


