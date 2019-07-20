#ifndef PARENT_LIST_H
#define PARENT_LIST_H

#include <map>
#include "eltern.h"

typedef std::map<int, eltern> parent_list;

bool from_db(parent_list& list, const std::string& db_connection);

#endif
