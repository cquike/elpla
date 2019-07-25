#include "befreiung.h" 

befreiung::befreiung(int parent_id, boost::gregorian::date start_time, 
                     std::string& cause) :
m_parent_id(parent_id),  m_period(start_time, boost::gregorian::pos_infin), 
m_cause(cause)
{
}

befreiung::befreiung(int parent_id, boost::gregorian::date start_time, 
                     boost::gregorian::date end_time, const std::string& cause) :
 m_parent_id(parent_id), m_period(start_time, end_time + boost::gregorian::date_duration(1)), 
 m_cause(cause) 
{
//  m_period.shift(boost::gregorian::days(1)); //The last day is not considered part of the period
}

int befreiung::parent_id() const
{
  return m_parent_id;
}

const boost::gregorian::date_period& befreiung::period() const
{
  return m_period;
}

const std::string& befreiung::cause()
{
  return m_cause;
}

