//TODO: license

#ifndef PARENTS_H
#define PARENTS_H


#include <vector>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include "waiver_list.h"
#include "closing_days.h"


class eltern
{

  public:

  eltern();

  eltern(int id, const std::string& name_kid, const std::string& name_mother, 
         const std::string& name_father, const std::string& email_mother, 
         const std::string& email_father, const std::string& password,
         const std::string& visit_hours,
         boost::gregorian::date_period membership_period);

  bool is_member_at(boost::gregorian::date date) const;

  bool is_member_at(std::string date);

  int get_available_days(boost::gregorian::date_period& period,
                                 waiver_list& waivers,
                                 closing_day_list& days_closed) const;

  int id() const;

  const std::string& name_kid() const;

  const std::string& name_mother() const;

  const std::string& name_father() const;

  const std::string& email_mother() const;

  const std::string& email_father() const;

  const std::string& password() const;

  const std::string& visit_hours() const;

  private:

  std::string m_name_kid;

  std::string m_name_mother;

  std::string m_name_father;

  std::string m_email_mother;

  std::string m_email_father;
  
  std::string m_password;

  std::string m_visit_hours;

  boost::gregorian::date_period m_membership_period;

  int m_id;
};

#endif
