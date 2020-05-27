#ifndef HS_KEYS_WRAPPER_H
#define HS_KEYS_WRAPPER_H

#if (defined _WIN32) || (defined _WIN64)
#define DECL_EXPORT __declspec(dllexport)
#define DECL_IMPORT __declspec(dllimport)
#else
#define DECL_EXPORT
#define DECL_IMPORT
#endif

#if defined(keys_wrapper_EXPORTS)
#  define keys_wrapper_EXPORT DECL_EXPORT
#else
#  define keys_wrapper_EXPORT DECL_IMPORT
#endif

#include <stdio.h>
#include <string>
#include <vector>
#include <keys/hs_keys.h>

using namespace std;

typedef struct keypairs {
	char privatekey[100];
	char pubkey[100];
	char pubkeyhash[100];
	char walletaddr[100];
} keypairs_type_t;

struct PrivateKeyHelpInfo {
	char privateKeyPrefix[10];
	char addrChecksum[10];
};

struct WalletAddrHelpInfo {
	char pubKeyAddrPrefix[10];
	char scriptAddrPrefix[10];
	char addrChecksum[10];
};

struct TestStruct {
	char stringData1[100];
	uint64_t ulongData;

	char stringData2[200];
	int32_t intData;
};

typedef struct ParamsHelperInfo {
	keypairs_type_t keypairs;
	struct PrivateKeyHelpInfo PrivHelper;
	struct WalletAddrHelpInfo AddrHelper;
	int	error;
}Params_type_t;

#ifdef __cplusplus
extern "C" {
#endif


struct PrivateKeyHelperConstant : public IPrivateKeyHelper {
        PrivateKeyHelperConstant(const char* privateKeyPrefix, const char* addrChecksum) :
                _privatekeyPrefix(ParseHex(privateKeyPrefix)) {
                _addrChecksumValue = parseHexToInt32Le(addrChecksum);
                }

        const std::vector<unsigned char> privkeyPrefix() const override {
                return _privatekeyPrefix;
        }

        int32_t addrChecksumValue() const override {
                return _addrChecksumValue;
        }

        vector<unsigned char> _privatekeyPrefix;
        int32_t _addrChecksumValue;
};

struct WalletAddrHelperConstant : public IWalletAddrHelper {
        WalletAddrHelperConstant(const char* pubkeyAddrPrefix, const char* scriptAddrPrefix, const char* addrChecksum) :
                _pubKeyAddrPrefix(ParseHex(pubkeyAddrPrefix)),
                _scriptAddrPrefix(ParseHex(scriptAddrPrefix)) {
                _addrChecksumValue = parseHexToInt32Le(addrChecksum);
                }

        const std::vector<unsigned char> pubkeyAddrPrefix() const override {
                return _pubKeyAddrPrefix;
        }

        const std::vector<unsigned char> scriptAddrPrefix() const override {
                return _scriptAddrPrefix;
        }

        int32_t addrChecksumValue() const override {
                return _addrChecksumValue;
        }

        vector<unsigned char> _pubKeyAddrPrefix;
        vector<unsigned char> _scriptAddrPrefix;
        int32_t _addrChecksumValue;
};



#ifdef _WIN32

keys_wrapper_EXPORT char* create_stream_publish_tx_shp(const char* streamKey, const char* streamItem, const char* createTxid,
	const char* unspentScriptPubKey, const char* unspentTxid, uint32_t unspentVOut,
	const char* unspentRedeemScript, const char* privateKey, struct PrivateKeyHelpInfo *helper);

keys_wrapper_EXPORT void create_key_pairs_shp(const struct PrivateKeyHelpInfo *privatehelper,
		const struct WalletAddrHelpInfo *addrhelper, keypairs_type_t *out);

keys_wrapper_EXPORT char *create_asset_send_tx_shp(const char *toAddr, double quantity, const char *issueTxid, int multiple,
		const char *unspentScriptPubKey, const char *unspentTxid, uint32_t unspentVout,
		double unspentQty, const char * unspentRedeemScript, const char *privateKey,
		struct PrivateKeyHelpInfo *privatehelper, struct WalletAddrHelpInfo *addrhelper);

keys_wrapper_EXPORT char *sign_message_shp(const char *strAddress, const char *strMessage,
		struct PrivateKeyHelpInfo *privatehelper, struct WalletAddrHelpInfo *addrhelper);

keys_wrapper_EXPORT int verify_message_shp(const char *strAddress, const char *strSign,
		const char *strMessage, struct WalletAddrHelpInfo *addrhelper);	
#endif		

/// createStreamPublishTx 함수를 c에서 사용 하기 위해 wrapping 한 함수.
keys_wrapper_EXPORT char* create_stream_publish_tx(const char* streamKey, const char* streamItem, const char* createTxid,
		const char* unspentScriptPubKey, const char* unspentTxid, uint32_t unspentVOut,
		const char* unspentRedeemScript, const char* privateKey, struct PrivateKeyHelpInfo *helper);
/// createKeyPairs 함수를 c에서 사용 하기 위해 wrapping 한 함수.
keys_wrapper_EXPORT keypairs_type_t *create_key_pairs(const struct PrivateKeyHelpInfo *privatehelper,
		const struct WalletAddrHelpInfo *addrhelper);
keys_wrapper_EXPORT KeyPairs create_keypairs(const struct PrivateKeyHelpInfo *privatehelper,
			  		const struct WalletAddrHelpInfo *addrhelper);

/// createpubKeybinarys 함수를 c에서 사용 하기 위해 wrapping 한 함수.
keys_wrapper_EXPORT unsigned char *create_pub_key_binarys(const struct PrivateKeyHelpInfo *privatehelper,
		const struct WalletAddrHelpInfo *addrhelper);
/// createAssetSendTx 함수를 c에서 사용 하기 위해 wrapping 한 함수.
keys_wrapper_EXPORT char *create_asset_send_tx(const char *toAddr, double quantity, const char *issueTxid, int multiple,
		const char *unspentScriptPubKey, const char *unspentTxid, uint32_t unspentVout,
		double unspentQty, const char * unspentRedeemScript, const char *privateKey,
		struct PrivateKeyHelpInfo *privatehelper, struct WalletAddrHelpInfo *addrhelper);
/// SignMessage 함수를 c에서 사용하기 위해 wrapping 한 함수.
keys_wrapper_EXPORT char *sign_message(const char *strAddress, const char *strMessage,
		struct PrivateKeyHelpInfo *privatehelper, struct WalletAddrHelpInfo *addrhelper);
keys_wrapper_EXPORT std::string sign_message_bin(const char *strAddress, const char *strMessage, size_t nMessageSize,
		struct PrivateKeyHelpInfo *privatehelper, struct WalletAddrHelpInfo *addrhelper);
/// VerifyMessage 함수를 c에서 사용하기 위해 wrapping 한 함수.
keys_wrapper_EXPORT int verify_message(const char *strAddress, const char *strSign,
		const char *strMessage, struct WalletAddrHelpInfo *addrhelper);
keys_wrapper_EXPORT int verify_message_bin(const char *strAddress, const char *strSign,
		const char *strMessage, const size_t nMessageSize, struct WalletAddrHelpInfo *addrhelper);

#ifdef __cplusplus
}
#endif

#endif	//HS_KEYS_WRAPPER_H
