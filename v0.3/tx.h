#ifndef __TX_H
#define __TX_H


#define TX_HEADER_MARK		0x7c484452	// "|HDR"


enum {
	// Token commands
	TX_CREATE_TOKEN		= 100,
	TX_SEND_TOKEN,

	// Channel commands
	TX_CREATE_CHANNEL	= 200,
	TX_PUBLISH_CHANNEL,
	TX_SUBSCRIBE_CHANNEL,

	// Smart contract commands
	TX_CREATE_CONTRACT	= 300,

	// Permission commands
	TX_GRANT_REVOKE		= 400,
	TX_DESTROY,

	// Wallet commands
	TX_CREATE_WALLET	= 500,
	TX_SET_WALLET,			// Set address to the wallet
	TX_LIST_WALLET,
	TX_CREATE_KEYPAIR,		// Create new keypair and display only

	// Other commands
	TX_CREATE_ACCOUNT	= 600,	// Account has several addresses
	TX_SET_ACCOUNT,
	TX_LIST_ACCOUNT,
};


typedef struct {
	string	pubkey;
	string	sign;
	double	sign_clock;
}	tx_sign_t;


typedef struct {
	string		from_addr;
	string		to_addr;
	string		token_name;	// 256 bytes

	uint64_t	quantity;	// ���� ���� 
	uint64_t	smallest_unit;	// �ּ� ���� 
	uint64_t	native_amount;	// native ����

	string		access;		// "1"=1time / "##"=N�� �߰� ���� ���� / "forever"=��� ���� ����
	time_t		start_time;	// token ���� �ð� (0�̸� ���)
	time_t		expire_time;	// token �ߴ� �ð� (0�̸� ���)

	string		user_data;
}	tx_create_token_t;


typedef struct {
	string		from_addr;
	string		to_addr;
	string		token_name;	// 256 bytes
	uint64_t	amount;
	uint64_t	native_amount;
	uint32_t	fee;

	string		user_data;
}	tx_send_token_t;


typedef struct {
	string		from_addr;
	string		to_addr;
	string		channel_name;	// 256 bytes

	string		access;		// "local" "permission" "anyone read" "anyone read write"
	time_t		start_time;	// channel ���� �ð� (0�̸� ���)
	time_t		expire_time;	// channel �ߴ� �ð� (0�̸� ���)

	string		user_data;
}	tx_create_channel_t;


typedef struct {
	string		from_addr;
	string		to_addr;
	string		channel_name;	// 256 bytes
	string		key;		// 256 bytes
	string		value;		// size limit: max tx size

	string		user_data;
}	tx_publish_channel_t;


typedef struct {
	string		from_addr;
	string		to_addr;
	string		contract_name;	// 256 bytes

	string		access;		// "local" "permission" "anyone"
	time_t		start_time;	// contract ���� �ð� (0�̸� ���)
	time_t		expire_time;	// contract �ߴ� �ð� (0�̸� ���)

	string		user_data;
}	tx_create_contract_t;


// Destroy object: master only
typedef struct {
	string		from_addr;
	string		to_addr;
	string		target_name;	// TOKEN / CHANNEL / CONTRACT
	string		action;		// "pause" "stop" "start" "destroy"

	string		user_data;
}	tx_destroy_t;


typedef struct {
	string		from_addr;
	string		to_addr;
	string		permission;	// TOKEN: issue, admin
					// CHANNEL: admin, read, write, 
					// CONTRACT: admin, read
					// WALLET: admin, read, send
					// ACCOUNT: admin, read

	time_t		start_time;	// permission ���� �ð� (0�̸� ���)
	time_t		expire_time;	// permission �ߴ� �ð� (0�̸� ���)

	string		user_data;
}	tx_grant_revoke_t;


typedef struct {
	string		from_addr;
	string		to_addr;
	string		wallet_name;

	string		access;		// "local" "permission" "anyone add delete"
	time_t		start_time;	// permission ���� �ð� (0�̸� ���)
	time_t		expire_time;	// permission �ߴ� �ð� (0�̸� ���)

	string		user_data;
}	tx_create_wallet_t;


typedef struct {
	string		from_addr;
	string		to_addr;
	string		account_name;

	string		access;		// "local" "permission" "anyone add delete"
	time_t		start_time;	// permission ���� �ð� (0�̸� ���)
	time_t		expire_time;	// permission �ߴ� �ð� (0�̸� ���)

	string		user_data;
}	tx_create_account_t;


// setaccount ADDR ACCOUNT	=> put wallet.db key=ACCOUNT::list value=ADDR ADDR ...
// listaccount ACCOUNT		=> get wallet.db key=ACCOUNT::list return=JSON array
// setwallet ADDR WALLET	=> put wallet.db key=WALLET::list value=ADDR ADDR ...
// listwallet WALLET		=> get wallet.db key=WALLET::list result=JSON array
// create keypair
typedef struct {
	string		from_addr;
	string		to_addr;
	string		command;	

	string		user_data;
}	tx_control_t;


typedef struct {
	uint32_t	mark;		// tx header mark (seperator)
	uint32_t	length;	
	tx_sign_t	sign;

	uint32_t	type;
	uint32_t	seq;

	union {
		tx_create_token_t	create_token;
		tx_send_token_t		send_token;
		tx_create_channel_t	create_channel;
		tx_publish_channel_t	publish_channel;
		tx_create_contract_t	create_contract;
		tx_destroy_t		destroy;
		tx_grant_revoke_t	grant_revoke;
		tx_create_wallet_t	create_wallet;
		tx_create_account_t	create_account;
		tx_control_t		control;
	} uu;

	uint32_t	sender_id;	// seed=01
	double		timeout_clock;	// network timeout clock (default: 10 sec)
	double		recv_clock;	// receive clock
	double		send_clock;	// send clock for consensus
	double		reply_clock;	//
	uint32_t	reply_ok;	// map �ʿ� 
	uint32_t	reply_fail;	// map �ʿ� 

}	tx_t;


#endif	// __TX_H
