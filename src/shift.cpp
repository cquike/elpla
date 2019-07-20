#include <boost/date_time/gregorian/gregorian.hpp>
#include "shift.h"

//TODO: namespace

shift::shift()
{
}

shift::shift(boost::gregorian::date when,
             const std::string& day_period,
             int parent_id,
             bool mandatory) :
  m_when(when), m_day_period(day_period), 
  m_parent_id(parent_id), m_mandatory(mandatory)
{
}

boost::gregorian::date shift::when() const
{
  return m_when;
}

const std::string& shift::day_period() const
{
  return m_day_period;
}

int shift::parent_id() const
{
  return m_parent_id;
}

bool shift::is_mandatory() const
{
  return m_mandatory;
}

void shift::set_mandatory(bool mandatory)
{
  m_mandatory = mandatory;
}
