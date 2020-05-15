#ifndef __TX_H
#define __TX_H


#include "txcommon.h"


#define TX_HEADER_MARK          0x7c484452      // "|HDR"


enum {
	STAT_VERIFY_OK		= 0x00000001,
	STAT_VERIFY_FAIL	= 0x00000000,

	STAT_INIT		= 0x00001000,
	STAT_VERIFY_REQUEST	= 0x00001001,
	STAT_VERI_RESULT	= 0x00001002,

	STAT_ERROR		= 0xFFFFFFFF,
	STAT_VALID		= 0x11111111,
};

enum {
	// Verification related - internal
	TX_VERIFY_REPLY		= 0x00007700,

        // Token commands
        TX_CREATE_TOKEN         = 1000,
        TX_SEND_TOKEN,

        // Channel commands
        TX_CREATE_CHANNEL       = 1200,
        TX_PUBLISH_CHANNEL,
        TX_SUBSCRIBE_CHANNEL,

        // Smart contract commands
        TX_CREATE_CONTRACT      = 1300,

        // Permission commands
        TX_GRANT_REVOKE         = 1400,
        TX_DESTROY,

        // Wallet commands
        TX_CREATE_WALLET        = 1500,
        TX_SET_WALLET,                  // Set address to the wallet
        TX_LIST_WALLET,
        TX_CREATE_KEYPAIR,              // Create new keypair and display only

        // Other commands
        TX_CREATE_ACCOUNT       = 1600,  // Account has several addresses
        TX_SET_ACCOUNT,
        TX_LIST_ACCOUNT,
};


typedef struct {
	uint32_t	nodeid;		// node id
	uint32_t        type;		// TX_xxx
	uint32_t        status;		// STAT_VERIFY_xx
	size_t		data_length;	// sign data length
	int		valid;		// 0=invalid 1=valid -1=none
	string		signature;	// signature of data
	string		txid;		// transaction id: sha256(sign)
}	tx_header_t;


typedef struct {
	string		not_userd_txid;
}	tx_verify_reply_t;


typedef struct {
	string          from_addr;
	string          to_addr;
	string          token_name;     // 256 bytes

	uint64_t        quantity;       // 발행 수량
	uint64_t        smallest_unit;  // 최소 단위
	uint64_t        native_amount;  // native 수량

	string          access;         // "1"=1time / "##"=N번 추가 생성 가능 / "forever"=계속 생성 가능
	time_t          start_time;     // token 시작 시간 (0이면 즉시)
	time_t          expire_time;    // token 중단 시간 (0이면 계속)

	string          user_data;
        double  	sign_clock;
}       tx_create_token_t;


typedef struct {
	string          from_addr;
	string          to_addr;
	string          token_name;     // 256 bytes
	double		amount;
	double		native_amount;
	double		fee;

	string          user_data;
        double  	sign_clock;
}       tx_send_token_t;


typedef struct {
        string          from_addr;
        string          to_addr;
        string          channel_name;   // 256 bytes

        string          access;         // "local" "permission" "anyone read" "anyone read write"
        time_t          start_time;     // channel 시작 시간 (0이면 즉시)
        time_t          expire_time;    // channel 중단 시간 (0이면 계속)

        string          user_data;
        double  	sign_clock;
}       tx_create_channel_t;


typedef struct {
        string          from_addr;
        string          to_addr;
        string          channel_name;   // 256 bytes
        string          key;            // 256 bytes
        string          value;          // size limit: max tx size

        string          user_data;
        double  	sign_clock;
}       tx_publish_channel_t;


typedef struct {
        string          from_addr;
        string          to_addr;
        string          contract_name;  // 256 bytes

        string          access;         // "local" "permission" "anyone"
        time_t          start_time;     // contract 시작 시간 (0이면 즉시)
        time_t          expire_time;    // contract 중단 시간 (0이면 계속)

        string          user_data;
        double  	sign_clock;
}       tx_create_contract_t;


// Destroy object: master only
typedef struct {
        string          from_addr;
        string          to_addr;
        string          target_name;    // TOKEN / CHANNEL / CONTRACT
        string          action;         // "pause" "stop" "start" "destroy"

        string          user_data;
        double  	sign_clock;
}       tx_destroy_t;


typedef struct {
        string          from_addr;
        string          to_addr;
        string          permission;     // TOKEN: issue, admin
                                        // CHANNEL: admin, read, write,
                                        // CONTRACT: admin, read
                                        // WALLET: admin, read, send
                                        // ACCOUNT: admin, read

        time_t          start_time;     // permission 시작 시간 (0이면 즉시)
        time_t          expire_time;    // permission 중단 시간 (0이면 계속)

        string          user_data;
        double  	sign_clock;
}       tx_grant_revoke_t;


typedef struct {
        string          from_addr;
        string          to_addr;
        string          wallet_name;

        string          access;         // "local" "permission" "anyone add delete"
        time_t          start_time;     // permission 시작 시간 (0이면 즉시)
        time_t          expire_time;    // permission 중단 시간 (0이면 계속)

        string          user_data;
        double  	sign_clock;
}       tx_create_wallet_t;


typedef struct {
        string          from_addr;
        string          to_addr;
        string          account_name;

        string          access;         // "local" "permission" "anyone add delete"
        time_t          start_time;     // permission 시작 시간 (0이면 즉시)
        time_t          expire_time;    // permission 중단 시간 (0이면 계속)

        string          user_data;
        double  	sign_clock;
}       tx_create_account_t;


// setaccount ADDR ACCOUNT      => put wallet.db key=ACCOUNT::list value=ADDR ADDR ...
// listaccount ACCOUNT          => get wallet.db key=ACCOUNT::list return=JSON array
// setwallet ADDR WALLET        => put wallet.db key=WALLET::list value=ADDR ADDR ...
// listwallet WALLET            => get wallet.db key=WALLET::list result=JSON array
// create keypair
typedef struct {
        string          from_addr;
        string          to_addr;
        string          command;

        string          user_data;
        double  	sign_clock;
}       tx_control_t;


typedef struct {

        uint32_t        sender_id;      // seed=01
        double          timeout_clock;  // network timeout clock (default: 10 sec)
        double          recv_clock;     // receive clock
        double          send_clock;     // send clock for consensus
        double          reply_clock;    //
        uint32_t        reply_ok;       // map 필요
        uint32_t        reply_fail;     // map 필요

}       txstat_t;


int	seriz_add(xserialize& xsz, tx_send_token_t& tx);
int	seriz_add(xserialize& xsz, tx_header_t& tx);
int	seriz_add(xserialize& xsz, tx_create_token_t& tx);
int	seriz_add(xserialize& xsz, tx_verify_reply_t& tx);

int	deseriz(xserialize& xsz, tx_send_token_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, tx_header_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, tx_create_token_t& tx, int dump = 0);
int	deseriz(xserialize& xsz, tx_verify_reply_t& tx, int dump = 0);


#endif  // __TX_H
