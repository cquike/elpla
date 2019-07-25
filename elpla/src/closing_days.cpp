#include <boost/date_time/gregorian/gregorian.hpp>
#include <cppdb/frontend.h>
#include "closing_days.h"

closing_day_list::closing_day_list(const std::string& db_connection) :
m_db_connection(db_connection)
{
}

void closing_day_list::remove(boost::gregorian::date& date)
{

  if(!is_closed(date));
    throw std::runtime_error("date is not in the closing days list");

  cppdb::session sql(m_db_connection);

  cppdb::statement st = sql << "DELETE from schliesstage where Tag = ?"
                            << boost::gregorian::to_tm(date);
  st.exec();
  
  auto match = std::find_if(begin(), end(),
             [&date](closing_day& d){return d.date() == date;});
  erase(match);
}

void closing_day_list::add(boost::gregorian::date& date,
                           const std::string& cause)
{
  cppdb::session sql(m_db_connection);

  cppdb::statement st = sql << "INSERT INTO schliesstage(Tag, Anlass) "
                        "VALUES(?,?)"
                        << boost::gregorian::to_tm(date) << cause;
  st.exec();
  
  int id_tbl = st.last_insert_id();
  push_back(closing_day(date, cause));
} 

bool closing_day_list::is_closed(const boost::gregorian::date& date)
{
  auto match = std::find_if(begin(), end(), 
               [&date](closing_day& d){return d.date() == date;});

  return match != end();
}

void closing_day_list::from_db()
{
  cppdb::session sql(m_db_connection);  

  cppdb::result res = sql << "SELECT Tag,Anlass "
                             "FROM schliesstage";

  clear();
  while(res.next()) 
  {
    std::string cause;
    std::tm date_t;
    res >> date_t >> cause;
    boost::gregorian::date date = boost::gregorian::date_from_tm(date_t);
    push_back(closing_day(date, cause));
  }
}

