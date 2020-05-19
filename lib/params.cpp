#include "params.h"


map<string, string> _params;


Params_type_t load_params(const string& Path)
{
	static Params_type_t Params;

	_params = mapFromFileReadAll(Path);

	snprintf(Params.PrivHelper.privateKeyPrefix, 
		sizeof(Params.PrivHelper.privateKeyPrefix), "%s", _params["private-key-version"].data());
	snprintf(Params.PrivHelper.addrChecksum, 
		sizeof(Params.PrivHelper.addrChecksum), "%s", _params["address-checksum-value"].data());
	snprintf(Params.AddrHelper.pubKeyAddrPrefix, 
		sizeof(Params.AddrHelper.pubKeyAddrPrefix), "%s", _params["address-pubkeyhash-version"].data());
	snprintf(Params.AddrHelper.scriptAddrPrefix, 
		sizeof(Params.AddrHelper.scriptAddrPrefix), "%s", _params["address-scripthash-version"].data());
	snprintf(Params.AddrHelper.addrChecksum, 
		sizeof(Params.AddrHelper.addrChecksum), "%s", _params["address-checksum-value"].data());

	return Params;
}

