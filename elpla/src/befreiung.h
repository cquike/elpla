#include <boost/date_time/gregorian/gregorian_types.hpp>

#ifndef BEFREIUNG_H
#define BEFREIUNG_H

class befreiung
{
  
  public:

  befreiung(int parent_id, boost::gregorian::date start_time, 
            std::string& cause);

  befreiung(int parent_id, boost::gregorian::date start_time, 
            boost::gregorian::date end_time, const std::string& cause);

  int parent_id() const;

  const boost::gregorian::date_period& period() const;

  const std::string& cause();

  private:
  
  boost::gregorian::date_period m_period;

  std::string m_cause;

  int m_parent_id;
};

#endif
