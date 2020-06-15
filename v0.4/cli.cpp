//
// Usage: cli IP:PORT
//
// Address: HUGUrwcFy1VC91nq7tRuZpaJqndoHDw64e
// Pubkey:  0256460f34a2e5405c899d35cbb5f71d2fe3081a07d3fcb47b6fb3797bf9e717a2
// Privkey: LWFutte4iZvna17pwecwExhQSFn1ic3EJeswcwhJMamvhqLafmgW
//
// Address: HEn5QNriNknvzee1D1YpLQaMpz9adCMoRZ
// Pubkey:  03e456e5896a4087284b169d65d1048845a7b1d45db370d499ed32cd6bcaf7a45a
// Privkey: LSehfPTP4ejs3Z1h37b2adw9yAVykiLFdKLWsiwuTXwZHsPZo5Dk
//

#include "txcommon.h"


const char *_from_privkey = "LRU9TP4iRbnJdrtzBrmw7C1u5i2njtGKLsLShjdw73FNW4oXM2Bj";
const char *_from_addr = "HBRPK7cf9LKTP5SyWrGbyKd1bVknSpzD8m";	
const char *_to_privkey = "LWbMWoSpChsrdqBBBaAmDVFekskVVt9UApzLMnrcYjLawh193DtA";
const char *_to_addr = "HP4rn9XGyYVrJtTTyqvmFpWAyEKe8tr9tQ";


Params_type_t _cliparams;


string create_token(string from_privkey, string from_addr, string to_addr, string token_name);
string send_token(string from_privkey, string from_addr, string to_addr, string token_name);
string create_channel(string from_privkey, string from_addr, string to_addr, string channel_name);
string publish_channel(string from_privkey, string from_addr, string channel_name, string key, string value);
string create_contract(string from_privkey, string from_addr, string to_addr, string contract_name, string program);
string destroy(string from_privkey, string from_addr, string type_name, string object_name, string action);
string grant(string from_privkey, string from_addr, string _to_addr, string type_name, string target_name, string perm);
string revoke(string from_privkey, string from_addr, string _to_addr, string type_name, string target_name, string perm);
string create_wallet(string from_privkey, string from_addr, string to_addr, string wallet_name);
string create_account(string from_privkey, string from_addr, string account_name);
string control(string from_privkey, string from_addr, string cmd, string arg1 = "", string arg2 = "", 
	string arg3 = "", string arg4 = "", string arg5 = "");

string make_header(int type, string _from_addr, string sbody);


int	main(int ac, char *av[])
{
	char	svr[256] = {0};

	if (ac >= 2)
	{
		snprintf(svr, sizeof(svr), "tcp://%s", av[1]);
	}
	else
	{
		snprintf(svr, sizeof(svr), "tcp://%s:%d", "192.168.1.11", DEFAULT_CLIENT_PORT);
	}

	printf("CLIENT: connect to %s (pid=%d)\n\n", svr, getpid());

	zmq::context_t context(1);

	zmq::socket_t requester(context, ZMQ_REQ);
	requester.connect(svr);

	// Load params for sign/verify
	_cliparams = load_params("../lib/params.dat");


	string data = create_token(_from_privkey, _from_addr, _to_addr, "");
	bool ret = s_send(requester, data);
	string reply = s_recv(requester);

	printf("CLIENT: Create COIN: reply=%s  ret=%d\n", reply.c_str(), ret);

/***
	data = create_token(_from_privkey, _from_addr, _to_addr, "XTOKEN");
	ret = s_send(requester, data);
	reply = s_recv(requester);

	printf("CLIENT: Create token XTOKEN: reply=%s  ret=%d\n", reply.c_str(), ret);

	for (int count = 0; count < 3; count++)
	{
		string data = send_token(_to_privkey, _to_addr, _from_addr, "");		// coin
		bool ret = s_send(requester, data);
		string reply = s_recv(requester);

#ifdef DEBUG
		printf("CLIENT: Send coin %7d: reply=%s  ret=%d\n",
			count + 1, reply.c_str(), ret);
#else
		if (count % 10000 == 0)
			printf("CLIENT: Send coin %7d: reply=%s  ret=%d\n", 
				count + 1, reply.c_str(), ret);
#endif
	}

	for (int count = 0; count < 5; count++)
	{
		string data = send_token(_to_privkey, _to_addr, _from_addr, "XTOKEN");
		bool ret = s_send(requester, data);
		string reply = s_recv(requester);

#ifdef DEBUG
		printf("CLIENT: Send token %7d: reply=%s  ret=%d\n",
			count + 1, reply.c_str(), ret);
#else
		if (count % 10000 == 0)
			printf("CLIENT: Send token %7d: reply=%s  ret=%d\n", 
				count + 1, reply.c_str(), ret);
#endif
	}

	data = create_channel(_from_privkey, _from_addr, _to_addr, "CH1");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send create_channel(CH1): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = create_channel(_from_privkey, _from_addr, _to_addr, "CH2");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send create_channel(CH1): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = publish_channel(_to_privkey, _to_addr, "CH1", "key1", "data1");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send publish_channel(CH1): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = publish_channel(_to_privkey, _to_addr, "CH2", "key2", "data2");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send publish_channel(CH2): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = create_contract(_from_privkey, _from_addr, _to_addr, "CT1", "program");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send create_contract(CT1, program): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = destroy(_from_privkey, _from_addr, "token", "XTOKEN", "start");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send destroy(token XTOKEN destroy): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = grant(_from_privkey, _from_addr, _to_addr, "chennel", "CH1", "admin,read,write");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send grant(from, to, channel, \"admin,read,write\"): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = revoke(_from_privkey, _from_addr, _to_addr, "chennel", "CH1", "read,write");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send revoke(from, to, chennel, \"admin,read,write\"): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = revoke(_from_privkey, _from_addr, _to_addr, "chennel", "CH1", "admin");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send revoke(from, to, chennel, \"admin,read,write\"): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = revoke(_from_privkey, _from_addr, _to_addr, "chennel", "CH1", "read,write");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send revoke(from, to, chennel, \"admin,read,write\"): reply=%s  ret=%d\n", reply.c_str(), ret);
	data = grant(_from_privkey, _from_addr, _to_addr, "chennel", "CH1", "admin,read,write");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send grant(from, to, channel, \"admin,read,write\"): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = create_wallet(_from_privkey, _from_addr, _to_addr, "MYWALLET");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send create_wallet(MYWALLET): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = create_account(_from_privkey, _from_addr, "ACC");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send create_account(ACC): reply=%s  ret=%d\n", reply.c_str(), ret);

	data = control(_from_privkey, _from_addr, "MYCOMMAND", "ARG1", "ARG2", "ARG3");
	ret = s_send(requester, data);
	reply = s_recv(requester);
	printf("CLIENT: Send control(MYCOMMAND, ARG1, ARG2, ARG3): reply=%s  ret=%d\n", reply.c_str(), ret);
***/
}


string make_header(int type, string privkey, string addr, string sbody)
{
	tx_header_t txhdr;

	txhdr.nodeid = getpid();	// 임시로 
	txhdr.type = type;
	txhdr.data_length = sbody.size();
	txhdr.from_addr = addr;
	txhdr.txclock = xgetclock();

	string shdr = txhdr.serialize();
	txhdr.signature = sign_message_bin(privkey.c_str(), sbody.c_str(), sbody.size(), 
				&_cliparams.PrivHelper, &_cliparams.AddrHelper);

	printf("%s:\n", get_type_name(type));
	printf("    Serialize: hdr  length=%ld\n", shdr.size());
	printf("    Serialize: body length=%ld\n", sbody.size());
	printf("    Address  : %s\n", addr.c_str());
	printf("    Signature: %s\n", txhdr.signature.c_str());

	// 발송 전에 미리 검증 테스트 
	int verify_check = verify_message_bin(addr.c_str(), txhdr.signature.c_str(), 
				sbody.c_str(), sbody.size(), &_cliparams.AddrHelper);
	printf("    verify_check=%d\n", verify_check);
	printf("\n");

	return shdr;
}


//
// 토큰 생성 명령
// - TX 기록 TXID 반환
// - leveldb에 엔트리 생성 (코인 관련)
//
string	create_token(string from_privkey, string from_addr, string to_addr, string token_name)
{
	static	int	count = 0;

	tx_header_t	txhdr;
	tx_create_token_t txcreate;
	char		tmp[256] = {0};

	count++;

	txcreate.from_addr = from_addr;
	txcreate.to_addr = to_addr;
	txcreate.token_name = token_name;
	txcreate.quantity = 100000000;
	txcreate.smallest_unit = 0.0001;
	txcreate.native_amount = 0.0;
	txcreate.fee = 0.0;
	sprintf(tmp, "{\"Creator\": \"Hyundai-pay\", \"Manager\": \"Lee Jae Min\"}");
	txcreate.user_data = tmp;

	string sbody = txcreate.serialize();

	string shdr = make_header(TX_CREATE_TOKEN, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string	send_token(string from_privkey, string from_addr, string to_addr, string token_name)
{
	tx_header_t	txhdr;
	tx_send_token_t	txsend;
	char		tmp[256] = {0};

	txsend.from_addr = from_addr;
	txsend.to_addr = to_addr;
	txsend.token_name = token_name;
	txsend.amount = 1.2345;
	txsend.native_amount = 0.0;
	txsend.fee = 0.0;
	sprintf(tmp, "{\"Comment\": \"Bigdata out-sourcing\"}");
	txsend.user_data = tmp;

	string sbody = txsend.serialize();

	string shdr = make_header(TX_SEND_TOKEN, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string	create_channel(string from_privkey, string from_addr, string to_addr, string channel_name)
{
	tx_header_t	txhdr;
	tx_create_channel_t	txchannel;
	char		tmp[256] = {0};

	txchannel.from_addr = from_addr;
	txchannel.to_addr = to_addr;
	txchannel.channel_name = channel_name;
	txchannel.access = "anyone read write";
	sprintf(tmp, "{\"Creator\": \"Hyundai-Pay\"}");
	txchannel.user_data = tmp;

	string sbody = txchannel.serialize();

	string shdr = make_header(TX_CREATE_CHANNEL, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string publish_channel(string from_privkey, string from_addr, string channel_name, string key, string value)
{
	tx_header_t	txhdr;
	tx_publish_channel_t	txpublish;
	char		tmp[256] = {0};

	txpublish.from_addr = from_addr;
	txpublish.channel_name = channel_name;
	txpublish.key = key;
	txpublish.value = value;
	sprintf(tmp, "{\"Creator\": \"Hyundai-Pay\"}");
	txpublish.user_data = tmp;

	string sbody = txpublish.serialize();

	string shdr = make_header(TX_PUBLISH_CHANNEL, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string create_contract(string from_privkey, string from_addr, string to_addr, string contract_name, string program)
{
	tx_header_t	txhdr;
	tx_create_contract_t	txpublish;
	char		tmp[256] = {0};

	txpublish.from_addr = from_addr;
	txpublish.to_addr = to_addr;
	txpublish.contract_name = contract_name;
	txpublish.program = program;
	txpublish.access = "anyone";
	sprintf(tmp, "{\"Creator\": \"Hyundai-Pay\"}");
	txpublish.user_data = tmp;

	string sbody = txpublish.serialize();

	string shdr = make_header(TX_CREATE_CONTRACT, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string destroy(string from_privkey, string from_addr, string type_name, string target_name, string action)
{
	tx_header_t	txhdr;
	tx_destroy_t	txdestroy;
	char		tmp[256] = {0};

	txdestroy.from_addr = from_addr;
	txdestroy.type_name = type_name;
	txdestroy.target_name = target_name;
	txdestroy.action = action;
	sprintf(tmp, "{\"Creator\": \"Hyundai-Pay\"}");
	txdestroy.user_data = tmp;

	string sbody = txdestroy.serialize();

	string shdr = make_header(TX_DESTROY, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string grant(string from_privkey, string from_addr, string to_addr, string type_name, string target_name, string perm)
{
	tx_header_t	txhdr;
	tx_grant_t	txgrant;
	char		tmp[256] = {0};

	txgrant.from_addr = from_addr;
	txgrant.to_addr = to_addr;
	txgrant.isgrant = true;
	txgrant.type_name = type_name;
	txgrant.target_name = target_name;
	txgrant.permission = perm;
	sprintf(tmp, "{\"Creator\": \"Hyundai-Pay\"}");
	txgrant.user_data = tmp;

	string sbody = txgrant.serialize();

	string shdr = make_header(TX_GRANT, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string revoke(string from_privkey, string from_addr, string to_addr, string type_name, string target_name, string perm)
{
	tx_header_t	txhdr;
	tx_grant_t	txrevoke;
	char		tmp[256] = {0};

	txrevoke.from_addr = from_addr;
	txrevoke.to_addr = to_addr;
	txrevoke.isgrant = false;
	txrevoke.type_name = type_name;
	txrevoke.target_name = target_name;
	txrevoke.permission = perm;
	sprintf(tmp, "{\"Creator\": \"Hyundai-Pay\"}");
	txrevoke.user_data = tmp;

	string sbody = txrevoke.serialize();

	string shdr = make_header(TX_REVOKE, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string create_wallet(string from_privkey, string from_addr, string to_addr, string wallet_name)
{
	tx_header_t	txhdr;
	tx_create_wallet_t	txwallet;
	char		tmp[256] = {0};

	txwallet.from_addr = from_addr;
	txwallet.to_addr = to_addr;
	txwallet.wallet_name = wallet_name;
	txwallet.access = "local";
	sprintf(tmp, "{\"Creator\": \"Hyundai-Pay\"}");
	txwallet.user_data = tmp;

	string sbody = txwallet.serialize();

	string shdr = make_header(TX_CREATE_WALLET, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string create_account(string from_privkey, string from_addr, string account_name)
{
	tx_header_t	txhdr;
	tx_create_account_t	txaccount;
	char		tmp[256] = {0};

	txaccount.from_addr = from_addr;
	txaccount.account_name = account_name;
	txaccount.access = "local";
	sprintf(tmp, "{\"Creator\": \"Hyundai-Pay\"}");
	txaccount.user_data = tmp;

	string sbody = txaccount.serialize();

	string shdr = make_header(TX_CREATE_ACCOUNT, from_privkey, from_addr, sbody);

	return shdr + sbody;
}


string control(string from_privkey, string from_addr, string cmd, string arg1, string arg2, string arg3, string arg4, string arg5)
{
	tx_header_t	txhdr;
	tx_control_t	txcontrol;
	char		tmp[256] = {0};

	txcontrol.from_addr = from_addr;
	txcontrol.command = cmd;
	txcontrol.arg1 = arg1;
	txcontrol.arg2 = arg2;
	txcontrol.arg3 = arg3;
	txcontrol.arg4 = arg4;
	txcontrol.arg5 = arg5;
	sprintf(tmp, "{\"Creator\": \"Hyundai-Pay\"}");
	txcontrol.user_data = tmp;

	string sbody = txcontrol.serialize();

	string shdr = make_header(TX_CONTROL, from_privkey, from_addr, sbody);

	return shdr + sbody;
}
