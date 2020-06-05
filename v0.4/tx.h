#ifndef __TX_H
#define __TX_H


#include "txcommon.h"


#define TX_HEADER_MARK		0x7c484452	// "|HDR"


enum {
	STAT_VERIFY_OK		= 0x00000001,
	STAT_VERIFY_FAIL	= 0x80000001,

	// mempool TX 복사 
	STAT_BCAST_TX		= 0x00000010,
	STAT_ADD_TO_MEMPOOL	= 0x00000020,

	// 블록 발행 전 검사 
	STAT_VERIFY_BLOCK_REQ	= 0x00100000,
	STAT_VERIFY_BLOCK_OK	= 0x00200000,
	STAT_VERIFY_BLOCK_FAIL	= 0x00400000,

	STAT_INIT		= 0x00001000,
	STAT_VERIFY_REQUEST	= 0x00001001,
	STAT_VERI_RESULT	= 0x00001002,

	STAT_ERROR		= 0xFFFFFFFF,
	STAT_VALID		= 0x11111111,

	FLAG_SENT_TX		= 0x01000000,	// 발송된 TX
	FLAG_TX_LOCK		= 0x02000000,	// 다음 블록체 포함될 TX
	FLAG_TX_DELETE		= 0x04000000,	// 삭제 
};

enum {
	// Block sync 
	TX_BLOCK_GEN_REQ	= 0x00010000,	// 블록 생성 요청 
	TX_BLOCK_GEN_REPLY	= 0x00020000,	// 블록 생성 요청에 대한 응답 
	TX_BLOCK_GEN		= 0x00040000,	// 블록 생성 명령 

	// Verification related - internal
	TX_VERIFY_REPLY		= 0x00007700,

	// Token commands
	TX_CREATE_TOKEN		= 1000,
	TX_SEND_TOKEN,

	// Channel commands
	TX_CREATE_CHANNEL	= 1200,
	TX_PUBLISH_CHANNEL,
	TX_SUBSCRIBE_CHANNEL,

	// Smart contract commands
	TX_CREATE_CONTRACT	= 1300,

	// Permission commands
	TX_GRANT_REVOKE		= 1400,
	TX_DESTROY,

	// Wallet commands
	TX_CREATE_WALLET	= 1500,
	TX_SET_WALLET,				// Set address to the wallet
	TX_LIST_WALLET,
	TX_CREATE_KEYPAIR,			// Create new keypair and display only

	// Other commands
	TX_CREATE_ACCOUNT	= 1600,		// Account has several addresses
	TX_SET_ACCOUNT,
	TX_LIST_ACCOUNT,
};


typedef struct block_info {
	block_info()
	{
		block_size = 0;
		block_height = 0;
		block_version = 0;
		block_clock = 0;
		block_numtx = 0;
	}
	size_t		block_size;		// 블록 크기 (자신을 제외한 아래 내용)
	string		block_hash;		// 블록 해시 sha256(block_info_t + orgdataser 리스트)
						// 아래의 내용에 대한 hash 값임 (자신은 0000으로 초기화된 상태)
	uint64_t	block_height;		// 블록 번호 
	uint64_t	block_version;		// 블록 버전 
	string		prev_block_hash;	// 이전 블록 hash
	double		block_clock;		// 블록 생성 시각 
	uint64_t	block_numtx;		// TX 개수 
	string		block_gen_addr;		// 블록 생성자 주소 
	string		block_signature;	// sign
}	block_info_t;


typedef struct tx_header {
	tx_header()
	{
		nodeid = 0;
		type = 0;
		data_length = 0;
		txclock = 0;
		recvclock = 0;

		block_height = 0;
		status = 0;
		valid = -1;
		flag = 0;
	}
	uint32_t	nodeid;		// node id
	uint32_t	type;		// TX_xxx
	size_t		data_length;	// sign data length
	string		signature;	// signature of data
	string		from_addr;	// 데이터 부분에 from_addr가 없는 경우에 사용
	double		txclock;	// tx generation clock
	double		recvclock;	// 수신 clock

	// 내부용: status는 노드 간 상태 전달용으로 사용함. 블록 저장시 초기화 필요함 
	size_t		block_height;	// 블록 번호
	uint32_t	status;		// STAT_VERIFY_xx
	int		valid;		// 0=invalid 1=valid -1=none
	string		txid;		// transaction id: sha256(sign)
	int		flag;		// FLAG_xxx bit mask
	int		value;
}	tx_header_t;


// 파일 저장용 TX
typedef struct file_tx_header {
	file_tx_header()
	{
		nodeid = 0;
		type = 0;
		data_length = 0;
		txclock = 0;
		recvclock = 0;
	}
	uint32_t	nodeid;		// node id
	uint32_t	type;		// TX_xxx
	size_t		data_length;	// sign data length
	string		signature;	// signature of data
	double		txclock;	// tx generation clock
	double		recvclock;	// 수신 clock
}	file_tx_header_t;


// tx 원본은 orgdataser에 저장
// sub.cpp에서 헤더는 hdrser 바디는 bodyser에 저장함
typedef struct {
	tx_header_t	hdr;		// original TX header (임시로 사용하는 값도 있음)
	string		hdrser;		// Serialized tx header for broadcast
	string		bodyser;	// Serialized tx body for sign/broadcast
	string		orgdataser;	// Serialized original hdader + body
}	txdata_t;


typedef struct {
	string		txid;
}	txid_info_req_t;


typedef struct tx_self_txid_info_reply {
	tx_self_txid_info_reply()
	{
		ntotal = 0;
		nfail = 0;
	}
	int		ntotal;		// 전체 요청 온 TXID 개수 
	int		nfail;		// 존재하지 않는 TXID 개수 
}	txid_info_reply_t;


typedef struct {
	string		sign_hash;
}	tx_sign_hash_t;


typedef struct {
	string		txid;
}	tx_verify_reply_t;


typedef struct tx_create_token {
	tx_create_token()
	{
		quantity = 0;
		smallest_unit = 0;
		native_amount = 0;

		start_time = 0;
		expire_time = 0;
	}
	string		from_addr;
	string		to_addr;
	string		token_name;	// 256 bytes

	uint64_t	quantity;	// 발행 수량
	uint64_t	smallest_unit;	// 최소 단위
	uint64_t	native_amount;	// native 수량
	double		fee;

	string		access;		// "1"=1time / "##"=N번 추가 생성 가능 / "0"=계속 생성 가능
	time_t		start_time;	// token 시작 시간 (0이면 즉시)
	time_t		expire_time;	// token 중단 시간 (0이면 계속)

	string		user_data;
}	tx_create_token_t;


typedef struct tx_send_token {
	tx_send_token()
	{
		amount = 0;
		native_amount = 0;
		fee = 0;
	}
	string		from_addr;
	string		to_addr;
	string		token_name;	// 256 bytes
	double		amount;
	double		native_amount;
	double		fee;

	string		user_data;
}	tx_send_token_t;


typedef struct tx_create_channel {
	tx_create_channel()
	{
		start_time = 0;
		expire_time = 0;
	}
	string		from_addr;
	string		to_addr;
	string		channel_name;	// 256 bytes

	string		access;		// "local" "permission" "anyone read" "anyone read write"
	time_t		start_time;	// channel 시작 시간 (0이면 즉시)
	time_t		expire_time;	// channel 중단 시간 (0이면 계속)

	string		user_data;
}	tx_create_channel_t;


typedef struct tx_publish_channel {
	string		from_addr;
	string		to_addr;
	string		channel_name;	// 256 bytes
	string		key;		// 256 bytes
	string		value;		// size limit: max tx size

	string		user_data;
}	tx_publish_channel_t;


typedef struct tx_create_contract {
	tx_create_contract()
	{
		start_time = 0;
		expire_time = 0;
	}
	string		from_addr;
	string		to_addr;
	string		contract_name;	// 256 bytes

	string		access;		// "local" "permission" "anyone"
	time_t		start_time;	// contract 시작 시간 (0이면 즉시)
	time_t		expire_time;	// contract 중단 시간 (0이면 계속)

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


typedef struct tx_grant_revoke {
	tx_grant_revoke()
	{
		start_time = 0;
		expire_time = 0;
	}
	string		from_addr;
	string		to_addr;
	string		permission;	// TOKEN: issue, admin
					// CHANNEL: admin, read, write,
					// CONTRACT: admin, read
					// WALLET: admin, read, send
					// ACCOUNT: admin, read

	time_t		start_time;	// permission 시작 시간 (0이면 즉시)
	time_t		expire_time;	// permission 중단 시간 (0이면 계속)

	string		user_data;
}	tx_grant_revoke_t;


typedef struct tx_create_wallet {
	tx_create_wallet()
	{
		start_time = 0;
		expire_time = 0;
	}
	string		from_addr;
	string		to_addr;
	string		wallet_name;

	string		access;		// "local" "permission" "anyone add delete"
	time_t		start_time;	// permission 시작 시간 (0이면 즉시)
	time_t		expire_time;	// permission 중단 시간 (0이면 계속)

	string		user_data;
}	tx_create_wallet_t;


typedef struct tx_create_account {
	tx_create_account()
	{
		start_time = 0;
		expire_time = 0;
	}
	string		from_addr;
	string		to_addr;
	string		account_name;

	string		access;		// "local" "permission" "anyone add delete"
	time_t		start_time;	// permission 시작 시간 (0이면 즉시)
	time_t		expire_time;	// permission 중단 시간 (0이면 계속)

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

	uint32_t	sender_id;	// seed=01
	double		timeout_clock;  // network timeout clock (default: 10 sec)
	double		recv_clock;	// receive clock
	double		send_clock;	// send clock for consensus
	double		reply_clock;	//
	uint32_t	reply_ok;	// map 필요
	uint32_t	reply_fail;	// map 필요

}	txstat_t;


int	seriz_add(xserialize& xsz, block_info_t& tx);
int	seriz_add(xserialize& xsz, txid_info_req_t& tx);
int	seriz_add(xserialize& xsz, txid_info_reply_t& tx);
int	seriz_add(xserialize& xsz, tx_sign_hash_t& tx);
int	seriz_add(xserialize& xsz, tx_send_token_t& tx);
int	seriz_add(xserialize& xsz, tx_header_t& tx);
int	seriz_add(xserialize& xsz, file_tx_header_t& tx);
int	seriz_add(xserialize& xsz, tx_create_token_t& tx);
int	seriz_add(xserialize& xsz, tx_verify_reply_t& tx);

int	deseriz(xserialize& xsz, block_info_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, txid_info_req_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, txid_info_reply_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, tx_sign_hash_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, tx_send_token_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, tx_header_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, file_tx_header_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, tx_create_token_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, tx_verify_reply_t& tx, int dump = 0);


#endif	// __TX_H
