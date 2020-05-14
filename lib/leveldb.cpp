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
	if (it_) leveldb_iter_destroy(it_);
	if (opt_) leveldb_options_destroy(opt_);
	if (ropt_) leveldb_readoptions_destroy(ropt_);
	if (wopt_) leveldb_writeoptions_destroy(wopt_);
//	if (cache_) leveldb_cache_destroy(cache_);
//	if (cmp_) leveldb_comparator_destroy(cmp_);
//	if (env_) leveldb_env_destroy(env_);

	if (ldb_) leveldb_close(ldb_); 
}


bool	leveldb::destroy(string dbname)
{
	char	*err = NULL;

	assert(ldb_ != NULL);

	leveldb_options_t *opt = leveldb_options_create();
	leveldb_destroy_db(opt, dbname.c_str(), &err);

	if (err != NULL)
	{
		fprintf(stderr, "ERROR: Leveldb %s destroy failed: %s\n", 
			dbname.c_str(), err);
		return false;
	}
	leveldb_free(err); 

	leveldb_options_destroy(opt);

	return true;
}


string	leveldb::get(const char *key, size_t keylen)
{
	char	*err = NULL;
	char	*readval = NULL;
	size_t	vallen = 0;

	assert(ldb_ != NULL);

	readval = leveldb_get(ldb_, ropt_, key, keylen, &vallen, &err);

	if (err != NULL) 
	{
		fprintf(stderr, "ERROR: Leveldb %s get() failed: %s\n", dbname_.c_str(), err);
		return NULL;
	}
	leveldb_free(err); 

	string retval(readval, vallen);

	return retval;
}


string	leveldb::get(string key)
{
	return get(key.c_str(), key.length());
}


bool	leveldb::put(const char *key, size_t keylen, const char *val, size_t vallen)	// for binary
{
	char	*err = NULL;

	assert(ldb_ != NULL);

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

	assert(ldb_ != NULL);

	leveldb_delete(ldb_, wopt_, key.c_str(), key.length(), &err);

	if (err != NULL) 
	{
		fprintf(stderr, "ERROR: Leveldb %s delete() failed: %s\n", dbname_.c_str(), err);
		return false;
	}
	leveldb_free(err); 

	return true;
}


void	leveldb::seek_first()
{ 
	assert(ldb_ != NULL);

	if (it_)
		leveldb_iter_destroy(it_);
	it_ = leveldb_create_iterator(ldb_, ropt_); 

	leveldb_iter_seek_to_first(it_);
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


