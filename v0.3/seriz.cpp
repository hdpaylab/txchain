//
// TX Serialize
//

#include "txcommon.h"


int	seriz_add(xserial& xsz, tx_send_token_t& tx)
{
	xsz << tx.type;
	xsz << tx.seq;
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


int	seriz_add(xserial& xsz, tx_sign_t& tx)
{
	xsz << tx.signature;
	xsz << tx.data_length;

	return 1;
}


int	seriz_add(xserial& xsz, tx_verify_reply_t& tx)
{
	xsz << tx.type;
	xsz << tx.status;
	xsz << tx.signature;
	xsz << tx.txid;

	return 1;
}


int	deseriz(xserial& xsz, tx_send_token_t& tx, int dump)
{
	xsz >> tx.type;
	xsz >> tx.seq;
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
		printf("type = %d\n", tx.type);
		printf("seq = %d\n", tx.seq);
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


int	deseriz(xserial& xsz, tx_sign_t& tx, int dump)
{
	xsz >> tx.signature;
	xsz >> tx.data_length;

	if (dump)
	{
		printf("sign = %s\n", tx.signature.c_str());
		printf("sign data length = %ld\n", tx.data_length);
	}

	return 1;
}


int	deseriz(xserial& xsz, tx_verify_reply_t& tx, int dump)
{
	xsz >> tx.type;
	xsz >> tx.status;
	xsz >> tx.signature;
	xsz >> tx.txid;

	if (dump)
	{
		printf("type = %d\n", tx.type);
		printf("status = %d\n", tx.status);
		printf("sign = %s\n", tx.signature.c_str());
		printf("txid = %s\n", tx.txid.c_str());
	}

	return 1;
}


