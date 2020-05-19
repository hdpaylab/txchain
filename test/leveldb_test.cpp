//
// Hdac code distributed under the GPLv3 license
//============================================================================================
// leveldb_test.c
//	Hdac leveldb test program
//
// History
//	2018/06/00	Initial code
//============================================================================================
//

//
// Performance test result
//   insert: put 1,000,000 items (key=256 bytes, value=1024 bytes)
//	db.sync(false)	: 13.6 sec (73.5k iops)
//
//   update: put 1,000,000 items (key=256 bytes, value=1024 bytes)
//	db.sync(false)	: 37.0 sec (27.0k iops)
//
//   search: get 1,000,000 items (key=256 bytes, value=1024 bytes)
//	db.sync(false)	:  3.8 sec (263.2k iops)
//
//   remove: remove 1,000,000 items (key=256 bytes, value=1024 bytes)
//	db.sync(false)	: 24.5 sec (40.6k iops)
//

#include <assert.h>
#include <unistd.h>
#include "leveldb.h"
#include "xserialize.h"


int	main(int ac, char *av[])
{
	leveldb db("level.db");

	db.put("version", "1.20");

	for (int ii = 0; ii < 10000; ii++)
	{
		char	key[32] = {0}, value[64] = {0};

		sprintf(key, "key %08d", ii);
		sprintf(value, "value %08d", ii);

		// binary data get/put test
		for (int nn = 0; nn < 64; nn++)
		{
			if (nn % 2 == 0)
				value[nn] = nn;
		}

		string kk = key;
		string vv(value, sizeof(value));

		printf("---PUT KEY=%s VALUE len=%ld\n", kk.c_str(), vv.length());
		db.put(kk, vv);

		string value2 = db.get(kk);
		printf("---GET KEY=%s VALUE2 len=%ld\n", kk.c_str(), value2.length());

		dumpbin(value2.c_str(), value2.length(), 1, 10);

		if (sizeof(value) != value2.length())
			printf("ERROR: value len=%ld  value2 len=%ld\n", sizeof(value), value2.length());
		else if (memcmp(value, value2.c_str(), sizeof(value)) != 0)
			printf("ERROR: memcmp(value, value2) failed!\n");
		else
			printf("OK: value == value2\n");

		if (ii % 100 == 0)
		{
			printf("%d...    \r", ii);
			fflush(stdout);
		}
		sleep(1);
	}
	printf("\n");

	string	value;

	value = db.get("version");
	cout << "get(version)=" << value << endl << endl;

	db.remove("version");

	value = db.get("version");
	cout << "get(version) after remove=" << value << endl << endl;

	printf("leveldb iterator test...\n");
	for (db.seek_first(); db.valid(); db.next())
	{
		cout << db.key() << " : " << db.value() << endl;
	}
	printf("END!\n");
}
