#include <boost/date_time/gregorian/gregorian.hpp>
#include <cppdb/frontend.h>
#include "waiver_list.h"

waiver_list::waiver_list(const std::string& db_connection) :
m_db_connection(db_connection)
{
}

void waiver_list::remove(int i)
{
  cppdb::session sql(m_db_connection);

  cppdb::statement st = sql << "DELETE from befreiung where ID = ?"<<i;
  st.exec();
  
  erase(i);
}

void waiver_list::add(int parent_id, boost::gregorian::date start_date,
                  boost::gregorian::date end_date, const std::string& cause)
{
  cppdb::session sql(m_db_connection);

  cppdb::statement st = sql << "INSERT INTO befreiung(Von,Bis,Eltern_ID,Grund) "
                        "VALUES(?,?,?,?)"
                        << boost::gregorian::to_tm(start_date) 
                        << boost::gregorian::to_tm(end_date) 
                        << parent_id << cause;
  st.exec();
  
  int id_tbl = st.last_insert_id();
  insert(std::make_pair(id_tbl, befreiung(parent_id, start_date, end_date, cause)));
} 

void waiver_list::from_db()
{
  cppdb::session sql(m_db_connection);  

  cppdb::result res = sql << "SELECT ID,Von,Bis,Eltern_ID,Grund "
                             "FROM befreiung";

  clear();
  while(res.next()) 
  {
    int id_tbl;
    std::string cause;
    int parent_id;
    std::tm start_date_t;
    std::tm end_date_t;
    res >> id_tbl >> start_date_t >> end_date_t >> parent_id >> cause;
    boost::gregorian::date start_date = boost::gregorian::date_from_tm(start_date_t);
    boost::gregorian::date end_date = boost::gregorian::date_from_tm(end_date_t);
    insert(std::make_pair(id_tbl, befreiung(parent_id, start_date, end_date, cause)));
  }
}

