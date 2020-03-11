#ifndef HS_HELPERS_H
#define HS_HELPERS_H

#include <iostream>
#include <map>

#include "KeysHelperWithRpc.h"
#include "KeysHelperWithFile.h"

std::map<std::string, std::string> mapFromFileReadAll(const std::string& Path);

#endif // HS_HELPERS_H
