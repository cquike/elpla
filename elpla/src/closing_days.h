#ifndef CLOSING_DAY_LIST_H
#define CLOSING_DAY_LIST_H

#include <map>
#include "closing_day.h"

class closing_day_list : public std::vector<closing_day>
{
  public:

  closing_day_list(const std::string& db_connection);

  void from_db();

  void remove(boost::gregorian::date& date);

  void add(boost::gregorian::date& date, const std::string& cause);
  
  bool is_closed(const boost::gregorian::date& date);

  private:
  
  std::string m_db_connection;
};

#endif
