// c++0x only, for std::bind
// #include <functional>

#include <boost/date_time/gregorian/gregorian.hpp>
#include "eltern.h"

//TODO: namespace

eltern::eltern() :
  m_membership_period(boost::gregorian::date(boost::gregorian::pos_infin),
                      boost::gregorian::date(boost::gregorian::pos_infin))
{
}

eltern::eltern(int id, const std::string& name_kid, const std::string& name_mother,
               const std::string& name_father, const std::string& email_mother,
               const std::string& email_father, const std::string& password,
               const std::string& visit_hours,
               boost::gregorian::date_period membership_period) :
  m_name_kid(name_kid), m_name_mother(name_mother), m_name_father(name_father),
  m_email_mother(email_mother), m_email_father(email_father), 
  m_password(password), m_visit_hours(visit_hours),
  m_membership_period(membership_period),
  m_id(id)
{
}

bool eltern::is_member_at(boost::gregorian::date date) const
{
  return m_membership_period.contains(date);
}

int eltern::get_available_days(boost::gregorian::date_period& period,
                               waiver_list& waivers,
                               closing_day_list& days_closed) const
{
  int available_days = 0;
  boost::gregorian::day_iterator ditr(period.begin());
  for (ditr = period.begin(); ditr <= period.end(); ++ditr)
  {
    if(is_member_at(*ditr) && !days_closed.is_closed(*ditr))
    {
      bool is_available = true;
      for (auto& waiver_id : waivers)
      {
        auto& waiver = waiver_id.second;
        if (waiver.parent_id() == m_id && waiver.period().contains(*ditr))
        {
          is_available = false;
          break;
        }
      }
      if(is_available)
        available_days++;
    }
  }

  return available_days;
}

int eltern::id() const
{
  return m_id;
}

const std::string& eltern::name_kid() const
{
  return m_name_kid;
}

const std::string& eltern::name_mother() const
{
  return m_name_mother;
}

const std::string& eltern::name_father() const
{
  return m_name_father;
}

const std::string& eltern::email_mother() const
{
  return m_email_mother;
}
  
const std::string& eltern::email_father() const
{
  return m_email_father;
}

const std::string& eltern::password() const
{
  return m_password;
}

const std::string& eltern::visit_hours() const
{
  return m_visit_hours;
}

