#include <memory>
#include <keys/hs_keys_wrapper.h>
#include <helpers/hs_helpers.h>
#include <map>

using namespace std;

ParamsHelperInfo paramsget(const string& Path)
{
	static ParamsHelperInfo Params;

	map<std::string, std::string> _params;

	_params = mapFromFileReadAll(Path);

	snprintf(Params.PrivHelper.privateKeyPrefix, 10, "%s", _params.at("private-key-version").data());
	snprintf(Params.PrivHelper.addrChecksum, 10, "%s", _params.at("address-checksum-value").data());
	snprintf(Params.AddrHelper.pubKeyAddrPrefix, 10, "%s", _params.at("address-pubkeyhash-version").data());
	snprintf(Params.AddrHelper.scriptAddrPrefix, 10, "%s", _params.at("address-scripthash-version").data());
	snprintf(Params.AddrHelper.addrChecksum, 10, "%s", _params.at("address-checksum-value").data());

	return Params;
}

