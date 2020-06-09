//
// Common functions
//


#include "txcommon.h"


double	update_balance(string token_name, string addr, double amount);


bool	cmd_create_token(txdata_t& txdata, tx_create_token_t& create_token)
{
	string key = "TOKEN::" + create_token.token_name;
	string token_name = _walletdb.get(key);
	if (token_name == "")
	{
		key = "TOKEN::" + create_token.token_name;
		string data = txdata.bodyser;
		_walletdb.put(key, data);
		printf("CREATE_TOKEN: write to WALLETDB: key=%s data=%ld\n", key.c_str(), data.size());
		double total = create_token.quantity / create_token.smallest_unit;

		// 수신 주소 잔액 조정 
		double final_balance = update_balance(create_token.token_name, create_token.to_addr, total);
		if (final_balance < 0)
			return false;

		printf("INITIAL_SEND_TOKEN: key=%s balance=%.15g\n", key.c_str(), total);
	}
	else
	{
		logprintf(0, "ERROR: Token '%s' exists!\n", create_token.token_name.c_str());
		return false;
	}
	return true;
}


bool	cmd_send_token(txdata_t& txdata, tx_send_token_t& send_token)
{
	string key = "TOKEN::" + send_token.token_name;
	string token_name = _walletdb.get(key);
	if (token_name == "")
	{
		logprintf(0, "ERROR: Token '%s' not found!\n", send_token.token_name.c_str());
		return false;
	}

	// 수신 주소 잔액 조정 
	double to_balance = update_balance(send_token.token_name, send_token.to_addr, send_token.amount);
	if (to_balance < 0)
		return false;

	// 발신 주소 잔액 조정 
	double from_balance = update_balance(send_token.token_name, send_token.from_addr, -send_token.amount);
	if (from_balance < 0)
	{
		// 실패시 원래대로 복귀 
		update_balance(send_token.token_name, send_token.to_addr, -send_token.amount);
		return false;
	}

	logprintf(0, "Send %s token ok: from=%s(after %.15g) send %.15g => to=%s final=%.15g\n",
		send_token.token_name.c_str(), send_token.from_addr.c_str(), from_balance,
		send_token.amount, send_token.to_addr.c_str(), to_balance);

	return true;
}


//
// addr 주소에 token_name 잔액을 amount만큼 조정함 
//
double	update_balance(string token_name, string addr, double amount)
{
	// 주소 잔액 확인 
	string key = addr + "::" + token_name;	
	double balance = 0;
	string sbalance = _walletdb.get(key);
	if (sbalance.size() > 0)
		balance = atof(sbalance.c_str());

	// 조정 후 잔액 계산 
	double final_balance = balance + amount;

	if (final_balance < 0)
	{
		logprintf(0, "ERROR: Send %s token failed: %s balance=%.15g send amount=%.15g\n",
			token_name.c_str(), addr.c_str(), balance, amount);
		return -1;
	}

	// 주소 송금 후 잔액 저장
	char stotal[32] = {0};
	sprintf(stotal, "%.15g", final_balance);
	_walletdb.put(key, stotal);

	return final_balance;
}
