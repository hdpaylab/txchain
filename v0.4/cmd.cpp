//
// Common functions
//


#include "txcommon.h"


bool	cmd_create_token(txdata_t& txdata, tx_create_token_t& create_token)
{
	string	key, data;

	string token_name = _walletdb.get(create_token.token_name);
	if (token_name == "")
	{
		logprintf(0, "ERROR: Token '%s' exists!\n", token_name);
		return false;
	}
	else
	{
		key = create_token.token_name;
		data = txdata.bodyser;
		_walletdb.put(key, data);
		printf("CREATE_TOKEN: write to WALLETDB: key=%s data=%ld\n",
			key.c_str(), data.size());
	}
	return true;
}


bool	cmd_send_token(txdata_t& txdata, tx_send_token_t& send_token)
{
	string	key, data;

	string token_name = _walletdb.get(send_token.token_name);
	if (token_name == "")
	{
		logprintf(0, "ERROR: Token '%s' not found!\n", send_token.token_name);
		return false;
	}
	else
	{
		key = token_name;
		data = txdata.bodyser;
	//	_txdb.put(key, data);
		printf("SEND_TOKEN: write to TXDB: key=%s data=%ld\n",
			key.c_str(), data.size());
	}
	return true;
}

