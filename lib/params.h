#ifndef __PARAMS_H
#define __PARAMS_H


#include <memory>
#include <keys/hs_keys_wrapper.h>
#include <helpers/hs_helpers.h>
#include <map>


using namespace std;


extern	map<string, string> _params;


Params_type_t load_params(const string& Path);


#endif	// __PARAMS_H
