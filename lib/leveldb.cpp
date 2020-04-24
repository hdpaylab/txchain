//
// Copyright (c) 2018 Hdac Technology AG
// Hdac code distributed under the GPLv3 license
//============================================================================================
// leveldb.c
//	Simple leveldb (hash db) class
//
// History
//	2018/06/05	Initial code
//============================================================================================
//

#include "leveldb.h"


using namespace std;


//
// Set up database connection information and open database
//
bool	leveldb::init(string dbname)
{
	char	*err = NULL;

	dbname_ = dbname;
	it_ = NULL;

	opt_ = leveldb_options_create();
	leveldb_options_set_create_if_missing(opt_, 1);

	ropt_ = leveldb_readoptions_create();
	wopt_ = leveldb_writeoptions_create();

	ldb_ = leveldb_open(opt_, dbname_.c_str(), &err);

	if (err != NULL)
	{
		fprintf(stderr, "ERROR: Unable to open/create leveldb %s: %s\n", 
			dbname_.c_str(), err);
		return false;
	}
	leveldb_free(err); 

	return true;
}


void	leveldb::close()
{
	if (ldb_) leveldb_close(ldb_); 
	if (it_) leveldb_iter_destroy(it_);
	if (opt_) leveldb_options_destroy(opt_);
	if (ropt_) leveldb_readoptions_destroy(ropt_);
	if (wopt_) leveldb_writeoptions_destroy(wopt_);
//	if (cache_) leveldb_cache_destroy(cache_);
//	if (cmp_) leveldb_comparator_destroy(cmp_);
//	if (env_) leveldb_env_destroy(env_);
}


bool	leveldb::destroy(string dbname)
{
	char	*err = NULL;

	leveldb_options_t *opt = leveldb_options_create();
	leveldb_destroy_db(opt, dbname.c_str(), &err);

	if (err != NULL)
	{
		fprintf(stderr, "ERROR: Leveldb %s destroy failed: %s\n", 
			dbname.c_str(), err);
		return false;
	}
	leveldb_free(err); 

	return true;
}


char	*leveldb::get(const char *key, size_t keylen, char **val, size_t *vallen)	// for binary
{
	char	*err = NULL;
	char	*readval = NULL;

	*val = NULL;

	readval = leveldb_get(ldb_, ropt_, key, keylen, vallen, &err);

	if (err != NULL) 
	{
		fprintf(stderr, "ERROR: Leveldb %s get() failed: %s\n", dbname_.c_str(), err);
		return NULL;
	}
	leveldb_free(err); 

	if (readval)
		*val = readval;

	return *val;
}


string	leveldb::get(string key)
{
	char	*readval = NULL;
	size_t	vallen = 0;
	string	retval;

	get(key.c_str(), key.length() + 1, &readval, &vallen);

	if (readval)
	{
		retval = readval;
		free(readval);
	}

	return retval;
}


bool	leveldb::put(const char *key, size_t keylen, const char *val, size_t vallen)	// for binary
{
	char	*err = NULL;

	leveldb_put(ldb_, wopt_, key, keylen, val, vallen, &err);

	if (err != NULL) 
	{
		fprintf(stderr, "ERROR: Leveldb %s put() failed: %s\n", dbname_.c_str(), err);
		return false;
	}
	leveldb_free(err); 

	return true;
}


bool	leveldb::remove(string key)
{
	char	*err = NULL;

	leveldb_delete(ldb_, wopt_, key.c_str(), key.length() + 1, &err);

	if (err != NULL) 
	{
		fprintf(stderr, "ERROR: Leveldb %s delete() failed: %s\n", dbname_.c_str(), err);
		return false;
	}
	leveldb_free(err); 

	return true;
}


//
// Iterate over each item in the database and print them
//
string	leveldb::key()
{
	const char *retkey = NULL;
	char	*err = NULL;
	size_t	keylen = 0;
	string	key;

	retkey = leveldb_iter_key(it_, &keylen);

	if (retkey == NULL) 
	{
		leveldb_iter_get_error(it_, &err);
		fprintf(stderr, "ERROR: Leveldb %s iter.key() failed: %s\n", dbname_.c_str(), err);
	}
	else
	{
		key = retkey;
	}

	return key;
}


//
// Iterate over each item in the database and print them
//
string	leveldb::value()
{
	const char *retvalue = NULL;
	char	*err = NULL;
	size_t	valuelen = 0;
	string	value;

	retvalue = leveldb_iter_value(it_, &valuelen);

	if (retvalue == NULL) 
	{
		leveldb_iter_get_error(it_, &err);
		fprintf(stderr, "ERROR: Leveldb %s iter.value() failed: %s\n", dbname_.c_str(), err);
	}
	else
	{
		value = retvalue;
	}

	return value;
}

