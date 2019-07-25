#include <boost/date_time/gregorian/gregorian.hpp>
#include <cppdb/frontend.h>
#include "shift_list.h"

int shift_list::count_assigned(const boost::gregorian::date_period& dates_range,
                               int parent_id, const std::string& day_period) const
{
  int num_assigned = std::count_if(begin(), end(), 
    [parent_id, &dates_range, &day_period](const shift& s)
      {return s.parent_id() == parent_id && dates_range.contains(s.when()) && s.day_period() == day_period;});
  return num_assigned;
}

int shift_list::count_assigned_mandatory(const boost::gregorian::date_period& dates_range,
                               int parent_id, const std::string& day_period) const
{
  int num_assigned = std::count_if(begin(), end(), 
    [parent_id, &dates_range, &day_period](const shift& s)
      {return s.parent_id() == parent_id && dates_range.contains(s.when()) && s.day_period() == day_period && s.is_mandatory() == true;});
  return num_assigned;
}

bool from_db(shift_list& shift_list, const std::string& db_connection)
{
  cppdb::session sql(db_connection);  

  cppdb::result res_shifts = sql << "SELECT Datum,TZ,Eltern_ID "
                             "FROM elterndienste";

  shift_list.clear();
  while(res_shifts.next()) 
  {
    std::string day_shift;
    int parent_id;
    std::tm date;
    res_shifts >> date >> day_shift >> parent_id ;
    boost::gregorian::date date_shift = boost::gregorian::date_from_tm(date);
    shift_list.push_back(shift(date_shift, day_shift, parent_id));
  }

  
  cppdb::session sql_mandatory(db_connection);  
  cppdb::result res_mandatory = sql_mandatory << "SELECT Datum,TZ "
                             "FROM ed_verpflichtend";

  while(res_mandatory.next())
  {
    std::string day_period;
    std::tm date;
    res_mandatory >> date >> day_period;
    boost::gregorian::date date_shift = boost::gregorian::date_from_tm(date);
    auto match = std::find_if(shift_list.begin(), shift_list.end(), 
               [&date_shift, &day_period](shift& s){return (s.when() == date_shift && s.day_period() == day_period);});

    if(match != shift_list.end())
      match->set_mandatory(true);
    else
      shift_list.push_back(shift(date_shift, day_period, -1));
  }
  return true;
}

