//
// TX Serialize
//

#include "txcommon.h"


////////////////////////////////////////////////////////////////////////////////
// Serialize

int	seriz_add(xserialize& xsz, tx_header_t& tx)
{
	xsz << tx.nodeid;		// node id
	xsz << tx.type;			// TX_xxx
	xsz << tx.status;		// STAT_VERIFY_xx
	xsz << tx.data_length;		// sign data length
	xsz << tx.valid;		// 0=invalid 1=valid -1=none
	xsz << tx.signature;		// signature of data

	return 1;
}


int	seriz_add(xserialize& xsz, tx_verify_reply_t& tx)
{
//	xsz << tx.txid;

	return 1;
}


int	seriz_add(xserialize& xsz, tx_create_token_t& tx)
{
	xsz << tx.from_addr;
	xsz << tx.to_addr;
	xsz << tx.token_name;     // 256 bytes

	xsz << tx.quantity;       // 발행 수량
	xsz << tx.smallest_unit;  // 최소 단위
	xsz << tx.native_amount;  // native 수량

	xsz << tx.access;         // "1"=1time / "##"=N번 추가 생성 가능 / "forever"=계속 생성 가능
	xsz << tx.start_time;     // token 시작 시간 (0이면 즉시)
	xsz << tx.expire_time;    // token 중단 시간 (0이면 계속)

	xsz << tx.user_data;
        xsz << tx.sign_clock;

	return 1;
}


int	seriz_add(xserialize& xsz, tx_send_token_t& tx)
{
	xsz << tx.from_addr;
	xsz << tx.to_addr;
	xsz << tx.token_name;
	xsz << tx.amount;
	xsz << tx.native_amount;
	xsz << tx.fee;

	xsz << tx.user_data;
	xsz << tx.sign_clock;

	return 1;
}


////////////////////////////////////////////////////////////////////////////////
// De-serialize

int	deseriz(xserialize& xsz, tx_header_t& tx, int dump)
{
	xsz >> tx.nodeid;		// node id
	xsz >> tx.type;			// TX_xxx
	xsz >> tx.status;		// STAT_VERIFY_xx
	xsz >> tx.data_length;		// sign data length
	xsz >> tx.valid;		// 0=invalid 1=valid -1=none
	xsz >> tx.signature;		// signature of data

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
	}

	return 1;
}


int	deseriz(xserialize& xsz, tx_verify_reply_t& tx, int dump)
{
//	xsz >> tx.txid;

	if (dump)
	{
//		printf("txid = %s\n", tx.txid.c_str());
	}

	return 1;
}


int	deseriz(xserialize& xsz, tx_create_token_t& tx, int dump)
{
	xsz >> tx.from_addr;
	xsz >> tx.to_addr;
	xsz >> tx.token_name;     // 256 bytes

	xsz >> tx.quantity;       // 발행 수량
	xsz >> tx.smallest_unit;  // 최소 단위
	xsz >> tx.native_amount;  // native 수량

	xsz >> tx.access;         // "1"=1time / "##"=N번 추가 생성 가능 / "forever"=계속 생성 가능
	xsz >> tx.start_time;     // token 시작 시간 (0이면 즉시)
	xsz >> tx.expire_time;    // token 중단 시간 (0이면 계속)

	xsz >> tx.user_data;
        xsz >> tx.sign_clock;

	if (dump)
	{
		printf("from_addr = %s\n", tx.from_addr.c_str());
		printf("to_addr = %s\n", tx.to_addr.c_str());
		printf("token_name = %s\n", tx.token_name.c_str());

		printf("quantity = %lu\n", tx.quantity);
		printf("smallest_unit = %lu\n", tx.smallest_unit);
		printf("native_amount = %lu\n", tx.native_amount);

		printf("access = %s\n", tx.access.c_str());
		printf("start_time = %ld\n", tx.start_time);
		printf("expire_time = %ld\n", tx.expire_time);

		printf("user_data = %s\n", tx.user_data.c_str());
		printf("sign_clock = %.3f\n", tx.sign_clock);
	}

	return 1;
}


int	deseriz(xserialize& xsz, tx_send_token_t& tx, int dump)
{
	xsz >> tx.from_addr;
	xsz >> tx.to_addr;
	xsz >> tx.token_name;
	xsz >> tx.amount;
	xsz >> tx.native_amount;
	xsz >> tx.fee;
	xsz >> tx.user_data;
	xsz >> tx.sign_clock;

	if (dump)
	{
		printf("from_addr = %s\n", tx.from_addr.c_str());
		printf("to_addr = %s\n", tx.to_addr.c_str());
		printf("token_name = %s\n", tx.token_name.c_str());
		printf("amount = %.6f\n", tx.amount);
		printf("native_amount = %.6f\n", tx.native_amount);
		printf("fee = %.6f\n", tx.fee);
		printf("user_data = %s\n", tx.user_data.c_str());
		printf("sign_clock = %.3f\n", tx.sign_clock);
	}

	return 1;
}


