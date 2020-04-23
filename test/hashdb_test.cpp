//
// Hdac code distributed under the GPLv3 license
//============================================================================================
// hashdb_test.c
//	Hdac hashdb test program
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

#include "hashdb.h"


int	main(int ac, char *av[])
{
	hashdb db("hash.db");

	db.put("version", "1.20");

//	db.sync(true);
//	db.sync(false);
	for (int ii = 0; ii < 10000; ii++)
	{
		char	tmp[64] = {0}, name[256] = {0}, value[1024] = {0};

/****
		memset(name, 'k', sizeof(name));
		name[sizeof(name)-1] = 0;
	//	name[rand() % 256] = 0;

		memset(value, 'v', sizeof(value));
		value[sizeof(value)-1] = 0;
	//	value[rand() % 1024] = 0;

		sprintf(tmp, "key %03d", ii);
		strncpy(name, tmp, strlen(tmp));
		sprintf(tmp, "value %03d", ii);
		strncpy(value, tmp, strlen(tmp));
****/
		db.put(name, value);
	//	db.remove(name);
		string value2 = db.get(name);
		if (value != value2)
			printf("value=%s(%ld)\nvalue2=%s(%ld)\n", value, strlen(value), value2.c_str(), value2.length());
		if (ii % 100 == 0)
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

	for (db.seek_first(); db.valid(); db.next())
	{
		cout << db.key() << " : " << db.value() << endl;
	}

	if (!db.ok())
	{
		cerr << "An error was found during the scan" << endl;
		cerr << db.status() << endl; 
	}
}
