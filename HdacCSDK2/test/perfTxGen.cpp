
#include <keys/hs_keys.h>
#include <helpers/KeysHelperWithRpc.h>
#include <rpc/rpcclient.h>
#include <chrono>
#include <utils/Profiler.h>

using namespace std;
using namespace chrono;

void testPerfTxGen()
{
	RpcClient client{ "52.78.28.69", 4332, "hdacrpc", "KCjzmJCu7VN1ZnLLSDawUR65MmrzE4nCUA9EwcvsjA5", "test" };
	KeysHelperWithRpc helper(client);

	auto keyPairsToSend = createKeyPairs(helper.privHelper(), helper.addrHelper());
	auto keyPairsToReceive = createKeyPairs(helper.privHelper(), helper.addrHelper());

	Profiler::instance().resumeBlock("createStreamPublishTx");
	//system_clock::time_point tp1 = system_clock::now();
	for (int i = 0; i < 10000; i++) {
		auto txForPublishStream = createStreamPublishTx("key1", "test to publish stream",
			"d920bc39cc3a934c81dabeeadf50774fd0ce95f89b9111d929a81fdcfd18b040",
			"76a914fec5bd6044c903efdd348132b6e8bb561614344a88ac1473706b700600000000000000ffffffffd329625c75",
			"a45b1acd08bd916472594b098607d76233dd8631643fbcde269d1cdf718a289f", 0,
			"",
			//"VHXjccrTPdRXG8asyos5oqvw6mhWtqASkbFsVuBnkpi4WXn2jr8eMwwp",
			"V95HczhRAyNTJRGXLDenz9sf5F4ycxfkRwmHTjmC5nShcsEjZnBnPcsy",
			helper.privHelper()
		);
	}
	//nanoseconds t = system_clock::now() - tp1;
	//cout << "nanoseconds: " << t.count() << endl;
	Profiler::instance().suspendBlock("createStreamPublishTx");

	//cout << "txForPublishStream : " << txForPublishStream << endl;

#if 0
	auto txForSendAsset = createAssetSendTx(keyPairsToReceive.walletAddr, 10,
		"44fdb8103f4e13d6ef2011d54933f2747b455c613b3cfe4886d187330d50b640", 10,
		"76a9143ab53060d41b5fa662a2d4575a69464b5759839588ac1c73706b7174f23349d51120efd6134e3f10b8fd44ac2600000000000075",
		"030374d736a70c5faf5d16887d2263e812cb896938bedeefd44c128417e2460a", 1,
		990.0,
		"",
		//"VHXjccrTPdRXG8asyos5oqvw6mhWtqASkbFsVuBnkpi4WXn2jr8eMwwp",
		keyPairsToSend.privateKey,
		helper.privHelper(),
		helper.addrHelper()
	);
	cout << "txForSendAsset : " << txForSendAsset << endl;
#endif
	Profiler::instance().printResult();
}

int main()
{
	testPerfTxGen();
	return 0;
}