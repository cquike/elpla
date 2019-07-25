#include "closing_day.h" 

closing_day::closing_day(const boost::gregorian::date date, 
                         const std::string& cause) :
m_date(date),
m_cause(cause)
{
}

const boost::gregorian::date& closing_day::date() const
{
  return m_date;
}

const std::string& closing_day::cause() const
{
  return m_cause;
}

