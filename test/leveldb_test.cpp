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

#include "leveldb.h"


int	main(int ac, char *av[])
{
	leveldb db("level.db");

	db.put("version", "1.20");

	for (int ii = 0; ii < 1000; ii++)
	{
		char	tmp[64] = {0}, key[16] = {0}, value[64] = {0};
/***
		memset(key, 'k', sizeof(key));
		key[sizeof(key)-2] = 0;

		memset(value, 'v', sizeof(value));
		value[sizeof(value)-2] = 0;

		sprintf(tmp, "key %03d", ii);
		strncpy(key, tmp, strlen(tmp));
		sprintf(tmp, "value %03d", ii);
		strncpy(value, tmp, strlen(tmp));
***/
		sprintf(key, "123k%2d", ii);
		sprintf(value, "123v%02d", ii);

		string kk = key;
		string vv = value;

		printf("---PUT KEY=%s VALUE=%s----\n", kk.c_str(), vv.c_str());
		db.put(kk, vv);

		string value2 = db.get(kk);
		printf("---GET KEY=%s VALUE=%s----\n", kk.c_str(), value2.c_str());

		if (value != value2)
			printf("ERROR: value=%s(%ld)\nvalue2=%s(%ld)\n", 
				value, strlen(value), value2.c_str(), value2.length());

		if (ii % 10000 == 0)
		{
			printf("%d...    \r", ii);
			fflush(stdout);
		}
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
}
