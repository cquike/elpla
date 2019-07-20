#include <boost/date_time/gregorian/gregorian_types.hpp>

#ifndef CLOSING_DAY_H
#define CLOSING_DAY_H

class closing_day
{
  
  public:

  closing_day(const boost::gregorian::date date, 
              const std::string& cause);

  const boost::gregorian::date& date() const;

  const std::string& cause() const;

  private:
  
  boost::gregorian::date m_date;

  std::string m_cause;
};

#endif
