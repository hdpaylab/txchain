#include <memory>
#include <keys/hs_keys_wrapper.h>
#include <helpers/hs_helpers.h>
#include <map>

using namespace std;

Params_type_t paramsget(const string& Path)
{
	static Params_type_t Params;

	map<std::string, std::string> _params;

	_params = mapFromFileReadAll(Path);

	snprintf(Params.PrivHelper.privateKeyPrefix, 
		sizeof(Params.PrivHelper.privateKeyPrefix), "%s", _params.at("private-key-version").data());
	snprintf(Params.PrivHelper.addrChecksum, 
		sizeof(Params.PrivHelper.addrChecksum), "%s", _params.at("address-checksum-value").data());
	snprintf(Params.AddrHelper.pubKeyAddrPrefix, 
		sizeof(Params.AddrHelper.pubKeyAddrPrefix), "%s", _params.at("address-pubkeyhash-version").data());
	snprintf(Params.AddrHelper.scriptAddrPrefix, 
		sizeof(Params.AddrHelper.scriptAddrPrefix), "%s", _params.at("address-scripthash-version").data());
	snprintf(Params.AddrHelper.addrChecksum, 
		sizeof(Params.AddrHelper.addrChecksum), "%s", _params.at("address-checksum-value").data());

	return Params;
}

