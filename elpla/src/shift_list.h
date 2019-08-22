#ifndef SHIFT_LIST_H
#define SHIFT_LIST_H

#include <map>
#include "shift.h"

class shift_list : public std::vector<shift>
{
  public:

  int count_assigned(const boost::gregorian::date_period& period, 
                     int parent_id, const std::string& day_shift) const;

  int count_assigned_mandatory(const boost::gregorian::date_period& period, 
                               int parent_id, const std::string& day_shift) const;

};


bool from_db(shift_list& list, const std::string& db_connection);

#endif
