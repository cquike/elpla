#ifndef ED_STATE_H
#define ED_STATE_H

#include <boost/date_time/gregorian/gregorian_types.hpp>

struct ed_state
{
    boost::gregorian::date closed_until;
    boost::gregorian::date opened_until;
    boost::gregorian::date opened_deadline;
};

bool from_db(ed_state& dest, const std::string& db_connection);

#endif
