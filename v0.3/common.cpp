//
// Common functions
//


#include "txcommon.h"


const char *get_type_name(int type)
{
	switch (type)
	{
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


const char *get_status_name(int status)
{
	switch (status)
	{
	case STAT_VERIFY_OK:		return "STAT_VERIFY_OK"; 
	case STAT_VERIFY_FAIL:		return "STAT_VERIFY_FAIL"; 

	case STAT_INIT:			return "STAT_INIT"; 
	case STAT_VERIFY_REQUEST:	return "STAT_VERIFY_REQUEST"; 
	case STAT_VERI_RESULT:		return "STAT_VERI_RESULT"; 

	case STAT_ERROR:		return "STAT_ERROR"; 
	case STAT_VALID:		return "STAT_VALID"; 
	default:
		printf("Unknown type %d", status); return "UNKNOWN";
	}
}
