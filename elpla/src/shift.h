//TODO: license

#ifndef SHIFT_H
#define SHIFT_H


#include <boost/date_time/gregorian/gregorian_types.hpp>

class shift
{

  public:

  shift();

  shift(boost::gregorian::date when,
        const std::string& day_shift, int parent_id,
        bool mandatory = false);

  boost::gregorian::date when() const;

  const std::string& day_period() const;

  int parent_id() const;
  
  bool is_mandatory() const;

  void set_mandatory(bool mandatory);

  private:

  boost::gregorian::date m_when;

  std::string m_day_period;
  
  int m_parent_id;
 
  bool m_mandatory;
};

#endif
