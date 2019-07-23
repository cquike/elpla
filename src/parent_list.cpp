#include <boost/date_time/gregorian/gregorian.hpp>
#include <cppdb/frontend.h>
#include "parent_list.h"

bool from_db(parent_list& parent_list, const std::string& db_connection)
{
  cppdb::session sql(db_connection);  

  cppdb::result res = sql << "SELECT ID,Name_Kind,Name_Mutter,Name_Vater,"
                             "Beginn_Mafalda,Ende_Mafalda,Mail_Mutter,Mail_Vater,Passwort,Visit_Hours "
                             "FROM eltern";

  parent_list.clear();
  while(res.next()) 
  {
    int id;
    std::string name_kid;
    std::string name_mother;
    std::string name_father;
    std::string email_mother;
    std::string email_father;
    std::string password;
    std::string visit_hours;
    std::tm start_date_tm;
    std::tm end_date_tm;
    res >> id >> name_kid >> name_mother >> name_father 
        >> start_date_tm >> end_date_tm >> email_mother >> email_father 
        >> password >> visit_hours;
    if(parent_list.count(id) != 0)
      throw std::runtime_error("duplicated parent id");
    boost::gregorian::date start_date = boost::gregorian::date_from_tm(start_date_tm);
    boost::gregorian::date end_date = boost::gregorian::date_from_tm(end_date_tm);
    if(end_date.year() >= 2038)
      end_date = boost::gregorian::date(boost::gregorian::pos_infin);
    //The last day is not considered part of the period, therefore adding one
    boost::gregorian::date_period membership_period(start_date, 
                                                    end_date + boost::gregorian::date_duration(1));
    
    parent_list[id] = eltern(id, name_kid, name_mother, name_father, 
                             email_mother, email_father, password, visit_hours,
                             membership_period);
  }
  return true;
}
