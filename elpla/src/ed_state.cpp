// c++0x only, for std::bind
// #include <functional>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <cppdb/frontend.h>
#include "ed_state.h"

//TODO: namespace

#define CLOSED_UNTIL "ClosedUntil"
#define OPENED_UNTIL "OpenedUntil"
#define OPENED_DEADLINE "OpenedDeadline"

static void check_defined(const std::string& msg, const boost::gregorian::date& to_check)
{
  if (to_check.is_not_a_date())
  {
    throw std::runtime_error(msg + " is not defined");
  }
}

bool from_db(ed_state& state, const std::string& db_connection)
{
  cppdb::session sql(db_connection);

  cppdb::result res = sql << "SELECT ID, Value FROM state";

  while(res.next())
  {
    std::string id;
    std::string value;
    res >> id >> value;

    if (id == CLOSED_UNTIL)
    {
      state.closed_until = boost::gregorian::from_string(value);
    }
    else if (id == OPENED_UNTIL)
    {
      state.opened_until = boost::gregorian::from_string(value);
    }
    else if (id == OPENED_DEADLINE)
    {
      state.opened_deadline = boost::gregorian::from_string(value);
    }
  }
  res.clear();

  check_defined(CLOSED_UNTIL, state.closed_until);
  check_defined(OPENED_UNTIL, state.opened_until);
  check_defined(OPENED_DEADLINE, state.opened_deadline);

  return true;
}
