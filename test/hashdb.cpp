//
// Copyright (c) 2018 Hdac Technology AG
// Hdac code distributed under the GPLv3 license
//============================================================================================
// hashdb.c
//	Simple leveldb (hash db) class
//
// History
//	2018/06/05	Initial code
//============================================================================================
//

#include "hashdb.h"


using namespace std;


//
// Set up database connection information and open database
//
void	hashdb::init(string dbname)
{
	it = NULL;

	leveldb::Options options;
	options.create_if_missing = true;

	leveldb::Status status = leveldb::DB::Open(options, dbname, &db);

	if (!status.ok())
	{
		cerr << "Unable to open/create hash database '" << dbname << "'" << endl;
		cerr << status.ToString() << endl;
		return;
	}
}


//
// Iterate over each item in the database and print them
//
void	hashdb::seek_first()
{
	if (it)
		delete it;

	it = db->NewIterator(leveldb::ReadOptions());

	it->SeekToFirst();
}


string	hashdb::get(string key)
{
	leveldb::ReadOptions options;
	string value;

	db->Get(options, key, &value);

	return value;
}


void	hashdb::put(string key, string value)
{
	db->Put(wopt, key, value);
}


void	hashdb::put(string key, int value)
{
	char	tmp[100] = {0};
	sprintf(tmp, "%d", value);
	db->Put(wopt, key, tmp);
}


void	hashdb::put(string key, double value)
{
	char	tmp[100] = {0};
	sprintf(tmp, "%.16g", value);
	db->Put(wopt, key, tmp);
}


void	hashdb::put(string key, long value)
{
	char	tmp[100] = {0};
	sprintf(tmp, "%ld", value);
	db->Put(wopt, key, tmp);
}


void	hashdb::remove(string key)
{
	db->Delete(wopt, key);
}


void	hashdb::sync(bool sync)
{
	wopt.sync = sync;
}
