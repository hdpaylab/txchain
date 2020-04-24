//
// Copyright (c) 2018 Hdac Technology AG
// Hdac code distributed under the GPLv3 license
//============================================================================================
// leveldb.h
//	Simple leveldb (hash db) class
//
// History
//	2018/06/05	Initial code
//============================================================================================
//

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>

#include "leveldb/c.h"


using namespace std;


class leveldb
{
private:
	string	dbname_;
	leveldb_t *ldb_;
	leveldb_iterator_t *it_;
	leveldb_options_t *opt_;
	leveldb_readoptions_t *ropt_;
	leveldb_writeoptions_t *wopt_;

public:
	leveldb(string dbname)
		{ init(dbname); }
	~leveldb()
		{ close(); }

	bool init(string dbname); 
	bool open(string dbname)
		{ return init(dbname); }
	void close();

	char *get(const char *key, size_t keylen, char **val, size_t *vallen);	// for binary
	bool put(const char *key, size_t keylen, const char *val, size_t vallen);	// for binary

	string get(string key);		// return value string
	bool put(string key, string value)
		{ return put(key.c_str(), key.length() + 1, value.c_str(), value.length() + 1); }
	bool put(string key, int value)
		{ return put(key.c_str(), key.length(), (const char *)&value, sizeof(value)); }
	bool put(string key, double value)
		{ return put(key.c_str(), key.length(), (const char *)&value, sizeof(value)); }
	bool put(string key, long value)
		{ return put(key.c_str(), key.length(), (const char *)&value, sizeof(value)); }

	bool remove(string key);
	bool destroy(string dbname);

	void seek_first();
	void next()
		{ leveldb_iter_next(it_); }
	void prev()
		{ leveldb_iter_prev(it_); }
	unsigned char valid()
		{ return leveldb_iter_valid(it_); }
	string key();
	string value();
};
