#ifndef ED_CONFIG_H
#define ED_CONFIG_H

#include <string>

class ed_config
{
private:
  bool check_file_permissions(const std::string& file);

public:
  ed_config();

  std::string availability_text;

  std::string assignment_text;

  std::string db_connection;
};

#endif
