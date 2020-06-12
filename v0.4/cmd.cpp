//
// Common functions
//


#include "txcommon.h"


double	update_balance(string token_name, string addr, double amount);
string	check_status(string key, string type, string name);


//
// createtoken command implementation
//
bool	cmd_create_token(txdata_t& txdata, tx_create_token_t& create_token)
{
	string key = "token::" + create_token.token_name;
	string token_name = _systemdb.get(key);
	if (token_name == "")
	{
		key = "token::" + create_token.token_name;
		string data = txdata.bodyser;
		_systemdb.put(key, data);

		_systemdb.put(key + "::STATUS", "start");

		printf("CREATE_TOKEN: write to SYSTEMDB: key=%s data=%ld\n", key.c_str(), data.size());
		double total = create_token.quantity / create_token.smallest_unit;

		// 수신 주소 잔액 조정 
		double final_balance = update_balance(create_token.token_name, create_token.to_addr, total);
		if (final_balance < 0)
			return false;

		printf("INITIAL_SEND_TOKEN: key=%s balance=%.15g\n", key.c_str(), total);
	}
	else
	{
		logprintf(0, "ERROR: Token '%s' already exists!\n", create_token.token_name.c_str());
		return false;
	}
	return true;
}


//
// pause, start, stop, destroy
//
string	check_status(string key, string type, string name)
{
	string stat = _systemdb.get(key + "::STATUS");
	if (stat == "destroy")
	{
		char	tmp[256] = {0};
		sprintf(tmp, "ERROR: %s '%s' destroyed!", type.c_str(), name.c_str());
		logprintf(0, "%s\n", tmp);
		return string(tmp);
	}
	else if (stat == "pause" || stat == "stop")
	{
		char	tmp[256] = {0};
		sprintf(tmp, "ERROR: %s '%s' stopped or paused!", type.c_str(), name.c_str());
		logprintf(0, "%s\n", tmp);
		return string(tmp);
	}
	else if (stat == "start")
	{
		return string("");
	}
	else
	{
		return string("");
	}
}


//
// sendtoken command implementation
//
bool	cmd_send_token(txdata_t& txdata, tx_send_token_t& send_token)
{
	string key = "token::" + send_token.token_name;
	string token_name = _systemdb.get(key);
	if (token_name == "")
	{
		if (send_token.token_name == "")
			logprintf(0, "ERROR: Native coin not found!\n");
		else
			logprintf(0, "ERROR: Token '%s' not found!\n", send_token.token_name.c_str());
		return false;
	}

	string errmsg = check_status(key, "token", send_token.token_name);
	if (errmsg != "")
	{
		txdata.hdr.txid = errmsg;
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

	logprintf(0, "Send token %s ok: from=%s(after %.15g) send %.15g => to=%s final=%.15g\n",
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
		logprintf(0, "ERROR: Send token %s failed: %s balance=%.15g send amount=%.15g\n",
			token_name.c_str(), addr.c_str(), balance, amount);
		return -1;
	}

	// 주소 송금 후 잔액 저장
	char stotal[32] = {0};
	sprintf(stotal, "%.15g", final_balance);
	_walletdb.put(key, stotal);

	return final_balance;
}


//
// createchannel command implementation
//
bool	cmd_create_channel(txdata_t& txdata, tx_create_channel_t& create_channel)
{
	string key = "channel::" + create_channel.channel_name;
	string channel_name = _systemdb.get(key);

	// 채널이 없으면 신규 생성 
	if (channel_name == "")
	{
		key = "channel::" + create_channel.channel_name;
		string data = txdata.bodyser;
		_systemdb.put(key, data);

		_systemdb.put(key + "::STATUS", "start");

		printf("CREATE_CHANNEL: write to SYSTEMDB: key=%s data=%ld\n", key.c_str(), data.size());
	}
	else
	{
		logprintf(0, "ERROR: Channel '%s' already exists!\n", create_channel.channel_name.c_str());
		return false;
	}
	return true;
}


//
// publishchannel command implementation
//
bool	cmd_publish_channel(txdata_t& txdata, tx_publish_channel_t& publish_channel)
{
	string key = "channel::" + publish_channel.channel_name;
	string channel_name = _systemdb.get(key);
	if (channel_name == "")
	{
		logprintf(0, "ERROR: Channel '%s' not found!\n", publish_channel.channel_name.c_str());
		return false;
	}

	string errmsg = check_status(key, "channel", publish_channel.channel_name);
	if (errmsg != "")
	{
		txdata.hdr.txid = errmsg;
		return false;
	}

	// 데이터 저장 
	key = publish_channel.channel_name + "::" + publish_channel.key;
	string data = txdata.bodyser;
	_txdb.put(key, data);

	logprintf(0, "Publish to channel %s ok: from=%s key=%s value=%s\n",
		publish_channel.channel_name.c_str(), publish_channel.from_addr.c_str(), 
		publish_channel.key.c_str(), publish_channel.value.c_str());

	return true;
}


//
// createcontract command implementation
//
bool	cmd_create_contract(txdata_t& txdata, tx_create_contract_t& create_contract)
{
	string key = "contract::" + create_contract.contract_name;
	string contract_name = _systemdb.get(key);

	// 채널이 없으면 신규 생성 
	if (contract_name == "")
	{
		key = "contract::" + create_contract.contract_name;
		string data = txdata.bodyser;
		_systemdb.put(key, data);

		_systemdb.put(key + "::STATUS", "start");

		printf("CREATE_CONTRACT: write to SYSTEMDB: key=%s data=%ld\n", key.c_str(), data.size());
	}
	else
	{
		logprintf(0, "ERROR: Contract '%s' already exists!\n", create_contract.contract_name.c_str());
		return false;
	}
	return true;
}


//
// destroy command implementation
//
bool	cmd_destroy(txdata_t& txdata, tx_destroy_t& destroy)
{
	if (destroy.action == "pause")
	{
		string key = destroy.type_name + "::" + destroy.target_name + "::STATUS";
		_systemdb.put(key, destroy.action);
	}
	else if (destroy.action == "start")
	{
		string key = destroy.type_name + "::" + destroy.target_name + "::STATUS";
		_systemdb.put(key, destroy.action);
	}
	else if (destroy.action == "stop")
	{
		string key = destroy.type_name + "::" + destroy.target_name + "::STATUS";
		_systemdb.put(key, destroy.action);
	}
	else if (destroy.action == "destroy")
	{
		string key = destroy.type_name + "::" + destroy.target_name + "::STATUS";
		_systemdb.put(key, destroy.action);

		key = destroy.type_name + "::" + destroy.target_name;
		_systemdb.remove(key);
	}

	string key = "destroy::" + destroy.type_name + "::" + destroy.target_name;
	string data = txdata.bodyser;
	_systemdb.put(key, data);

	printf("DESTROY: write to SYSTEMDB: key=%s data=%ld\n", key.c_str(), data.size());

	return true;
}


void	dump_systemdb()
{
	for (_systemdb.seek_first(); _systemdb.valid(); _systemdb.next())
	{
		cout << "DUMP SYSTEMDB: " << _systemdb.key() << " : " << _systemdb.value() << endl;
	}
	cout << endl;
}


//
// op: "+"는 old + cur / "-"는 old에서 cur의 퍼미션을 제거함 
// old, cur: 이전 퍼미션, 현재 퍼미션 
//
string	merge_perm(string op, string old, string cur)
{
	vector<string> oldv = split_string(old, ",");
	vector<string> curv = split_string(cur, ",");
	string perm;

	for (string str : oldv)
	{
		if (str == "")
			continue;

		if (op == "-")
		{
			// 제외 목록에 있으면 skip
			if (strstr(cur.c_str(), str.c_str()) != NULL)
			{
				continue;
			}
			// 제외 목록에 없으면 추가 
			else
			{
				if (perm == "")
					perm += str;
				else
					perm += "," + str;
			}
		}
		// 현재 perm에 없으면 추가 
		else if (op == "+" && strstr(perm.c_str(), str.c_str()) == NULL)
		{
			if (perm == "")
				perm += str;
			else
				perm += "," + str;
		}
	}

	for (string str : curv)
	{
		if (op == "-" || str == "")
			continue;
		// 현재 perm에 없으면 추가 
		if (op == "+" && strstr(perm.c_str(), str.c_str()) == NULL)
		{
			if (perm == "")
				perm += str;
			else
				perm += "," + str;
		}
	}

	printf("old=%s cur=%s FINAL perm=%s\n", old.c_str(), cur.c_str(), perm.c_str());

	return perm;
}


//
// grant command implementation
//
bool	cmd_grant_revoke(txdata_t& txdata, tx_grant_t& grant)
{
	if (grant.isgrant)
	{
		// 일단 DB 기록 
		string key = "grant::" + grant.type_name + "::" + grant.target_name + "::" + grant.to_addr;
		string data = txdata.bodyser;
		_systemdb.put(key, data);

		// 기존 퍼미션 입수함 
		key = grant.type_name + "::" + grant.target_name + "::" + grant.to_addr;
		string perm = _systemdb.get(key);

		// 기존 퍼미션과 합친 후 다시 DB 기록 
		string newperm = merge_perm("+", perm, grant.permission);
		key = grant.type_name + "::" + grant.target_name + "::" + grant.to_addr;
		_systemdb.put(key, newperm);

		printf("GRANT: write to SYSTEMDB: key=%s perm=%s data=%ld\n", 
			key.c_str(), newperm.c_str(), data.size());
	}
	else
	{
		// 일단 DB 기록 
		string key = "revoke::" + grant.type_name + "::" + grant.target_name + "::" + grant.to_addr;
		string data = txdata.bodyser;
		_systemdb.put(key, data);

		key = grant.type_name + "::" + grant.target_name + "::" + grant.to_addr;
		string perm = _systemdb.get(key);

		// 기존 퍼미션 perm에서 revoke하는 퍼미션을 제거 후 DB 기록
		// 퍼미션 내용이 없으면 엔트리 삭제함 
		string newperm = merge_perm("-", perm, grant.permission);
		if (newperm == "")
			_systemdb.remove(key);
		else
			_systemdb.put(key, newperm);

		printf("REVOKE: write to SYSTEMDB: remove key=%s perm=%s data=%ld\n", 
			key.c_str(), newperm.c_str(), data.size());
	}

	return true;
}


//
// createwallet command implementation
//
bool	cmd_create_wallet(txdata_t& txdata, tx_create_wallet_t& create_wallet)
{
	string key = "wallet::" + create_wallet.wallet_name;
	string wallet_name = _systemdb.get(key);

	// 채널이 없으면 신규 생성 
	if (wallet_name == "")
	{
		key = "wallet::" + create_wallet.wallet_name;
		string data = txdata.bodyser;
		_systemdb.put(key, data);

		printf("CREATE_WALLET: write to SYSTEMDB: key=%s data=%ld\n", key.c_str(), data.size());
	}
	else
	{
		logprintf(0, "ERROR: Wallet '%s' already exists!\n", create_wallet.wallet_name.c_str());
		return false;
	}
	return true;
}


//
// createaccount command implementation
//
bool	cmd_create_account(txdata_t& txdata, tx_create_account_t& create_account)
{
	string key = "account::" + create_account.account_name;
	string account_name = _systemdb.get(key);

	// 채널이 없으면 신규 생성 
	if (account_name == "")
	{
		key = "account::" + create_account.account_name;
		string data = txdata.bodyser;
		_systemdb.put(key, data);

		printf("CREATE_ACCOUNT: write to SYSTEMDB: key=%s data=%ld\n", key.c_str(), data.size());
	}
	else
	{
		logprintf(0, "ERROR: Account '%s' already exists!\n", create_account.account_name.c_str());
		return false;
	}
	return true;
}


//
// control command implementation
//
bool	cmd_control(txdata_t& txdata, tx_control_t& control)
{
	string key = "control::" + control.from_addr;
	string data = txdata.bodyser;
	tx_control_t ctl;

	strdeseriz(data, ctl, 0);

	_systemdb.put(key, data);
	printf("CREATE_CONTROL: write to SYSTEMDB: key=%s data=%ld cmd=%s %s %s %s %s %s\n",
		key.c_str(), data.size(), ctl.command.c_str(), ctl.arg1.c_str(),
		ctl.arg2.c_str(), ctl.arg3.c_str(), ctl.arg4.c_str(), ctl.arg5.c_str());

	return true;
}


