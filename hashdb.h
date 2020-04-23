//
// Copyright (c) 2018 Hdac Technology AG
// Hdac code distributed under the GPLv3 license
//============================================================================================
// hashdb.h
//	Simple leveldb (hash db) class
//
// History
//	2018/06/05	Initial code
//============================================================================================
//

#include <iostream>
#include <sstream>
#include <string>

#include "leveldb/db.h"


using namespace std;


class hashdb
{
private:
	leveldb::DB* db;
	leveldb::Iterator *it;
	leveldb::WriteOptions wopt; 

public:
	hashdb(string dbname)
		{ init(dbname); }
	~hashdb()
		{ if (it) delete it; delete db; db = NULL; }

	void init(string dbname); 

	string get(string key);
	void put(string key, string value);
	void put(string key, int value);
	void put(string key, double value);
	void put(string key, long value);
	void remove(string key);
	void sync(bool sync);

	void seek_first();
	bool valid()
		{ return it ? it->Valid() : false; }
	void next()
		{ if (it) it->Next(); }
	string key()
		{ return it ? it->key().ToString() : ""; }
	string value()
		{ return it ? it->value().ToString() : ""; }
	bool ok()
		{ return it ? it->status().ok() : false; }
	string status()
		{ return it ? it->status().ToString() : ""; }
};
