#ifndef ED_CONFIG_H
#define ED_CONFIG_H

#include <string>
#include <boost/program_options.hpp>

class ed_config
{
private:
  bool check_file_permissions(const std::string& file);
  boost::program_options::options_description config;
  boost::program_options::variables_map vm;

public:
  ed_config();

  template<class T>
  const T& get(const std::string& key) const;

  std::string db_connection;

  std::string system_url;

  std::string system_mainteners;

  std::string availability_text;

  std::string assignment_text;

  std::string email_notifier;

  std::string email_notifier_name;

  std::string smtp_server;

  std::string smtp_user;

  std::string smtp_password;
};

#endif
