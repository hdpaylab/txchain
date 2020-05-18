//
// Common functions
//


#include "txcommon.h"


const char *get_status_name(int status)
{
	switch (status)
	{
	case STAT_VERIFY_OK:		return "STAT_VERIFY_OK";
	case STAT_VERIFY_FAIL:		return "STAT_VERIFY_FAIL";

	// mempool TX 복사 
	case STAT_BCAST_TX:		return "STAT_BCAST_TX";
	case STAT_ADD_TO_MEMPOOL:	return "STAT_ADD_TO_MEMPOOL";

	// 블록 발행 전 검사 
	case STAT_VERIFY_BLOCK_REQ:	return "STAT_VERIFY_BLOCK_REQ";
	case STAT_VERIFY_BLOCK_OK:	return "STAT_VERIFY_BLOCK_OK";
	case STAT_VERIFY_BLOCK_FAIL:	return "STAT_VERIFY_BLOCK_FAIL";

	case STAT_INIT:			return "STAT_INIT";
	case STAT_VERIFY_REQUEST:	return "STAT_VERIFY_REQUEST";
	case STAT_VERI_RESULT:		return "STAT_VERI_RESULT";

	case STAT_ERROR:		return "STAT_ERROR";
	case STAT_VALID:		return "STAT_VALID";

	case FLAG_SENT_TX:		return "FLAG_SENT_TX";
	case FLAG_NEXT_BLOCK:		return "FLAG_NEXT_BLOCK";

	default:
		printf("Unknown type %d", status); return "UNKNOWN";
	}
}


const char *get_type_name(int type)
{
	switch (type)
	{
	case TX_BLOCK_SYNC_REQ:		return "TX_BLOCK_SYNC_REQ"; 
	case TX_BLOCK_SYNC_REPLY:	return "TX_BLOCK_SYNC_REPLY"; 

	case TX_VERIFY_REPLY:		return "TX_VERIFY_REPLY"; 

        case TX_CREATE_TOKEN:		return "TX_CREATE_TOKEN"; 
        case TX_SEND_TOKEN:		return "TX_SEND_TOKEN"; 

        case TX_CREATE_CHANNEL:		return "TX_CREATE_CHANNEL"; 
        case TX_PUBLISH_CHANNEL:	return "TX_PUBLISH_CHANNEL"; 
        case TX_SUBSCRIBE_CHANNEL:	return "TX_SUBSCRIBE_CHANNEL"; 

        case TX_CREATE_CONTRACT:	return "TX_CREATE_CONTRACT"; 

        case TX_GRANT_REVOKE:		return "TX_GRANT_REVOKE"; 
        case TX_DESTROY:		return "TX_DESTROY"; 

        case TX_CREATE_WALLET:		return "TX_CREATE_WALLET"; 
        case TX_SET_WALLET:		return "TX_SET_WALLET"; 
        case TX_LIST_WALLET:		return "TX_LIST_WALLET"; 
        case TX_CREATE_KEYPAIR:		return "TX_CREATE_KEYPAIR"; 

        case TX_CREATE_ACCOUNT:		return "TX_CREATE_ACCOUNT"; 
        case TX_SET_ACCOUNT:		return "TX_SET_ACCOUNT"; 
        case TX_LIST_ACCOUNT:		return "TX_LIST_ACCOUNT"; 
	default:
		printf("Unknown type %d", type); return "UNKNOWN";
	}
}


tx_header_t	*parse_header_body(txdata_t& txdata)
{
	xserialize hdrszr, bodyszr;

	hdrszr.setstring(txdata.orgdataser);
	deseriz(hdrszr, txdata.hdr, 0);		// 헤더 부분 먼저 deserialize

	string body = hdrszr.getcurstring();	// 헤더 뒷부분: body
	bodyszr.setstring(body);		// body 부분 

	txdata.bodyser = body;

	xserialize tmpszr;
	seriz_add(tmpszr, txdata.hdr);
	txdata.hdrser = tmpszr.getstring();	// 헤더 serialization 교체

	return &txdata.hdr;
}
