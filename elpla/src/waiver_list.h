#ifndef WAIVER_LIST_H
#define WAIVER_LIST_H

#include <map>
#include "befreiung.h"

class waiver_list : public std::map<int, befreiung>
{
  public:

  waiver_list(const std::string& db_connection);

  void from_db();

  void remove(int i);

  void add(int parent_id, boost::gregorian::date start_date,
           boost::gregorian::date end_date, const std::string& cause);

  private:
  
  std::string m_db_connection;
};

#endif
