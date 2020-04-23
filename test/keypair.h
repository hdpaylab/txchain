#ifndef __KEYPAIR_H
#define __KEYPAIR_H


#include <vector>
#include <iostream>
#include <string>
#include <stdint.h>
#include <keys/hs_keys_wrapper.h>
#include <helpers/hs_helpers.h>
#include <keys/keyshelper.h>
#include <keys/key.h>
#include <keys/bitcoinsecret.h>
#include <rpc/rpcclient.h>
#include <keys/eccautoinitreleasehandler.h>
#include <keys/bitcoinaddress.h>
#include <utils/utilstrencodings.h>
#include <stdio.h>
#include <stdlib.h>


using namespace std;


// private-key-version = 83                # Version bytes used for exporting private keys.
// address-checksum-value = 48444143       # Bytes used for XOR in address checksum calculation.
extern struct PrivateKeyHelpInfo privinfo;

// address-pubkeyhash-version = 28         # Version bytes used for pay-to-pubkeyhash addresses.
// address-scripthash-version = 08         # Version bytes used for pay-to-scripthash addresses.
// address-checksum-value = 48444143       # Bytes used for XOR in address checksum calculation.
extern struct WalletAddrHelpInfo addrinfo;


#endif	// __KEYPAIR_H
