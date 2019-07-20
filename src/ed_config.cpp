#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <string>
#include "ed_config.h"
#include <boost/program_options.hpp>
namespace po = boost::program_options;


ed_config::ed_config() :
  db_connection("mysql:"),
  availability_text(),
  assignment_text()
{

  std::string etcrc = "/etc/elpla/elpla.rc";
  const char *homedir;

  if ((homedir = getenv("HOME")) == NULL) 
    throw std::runtime_error("Cannot get home dir");

  std::string homerc = std::string(homedir)+"/.elpla/elpla.rc";

  po::options_description config("Configuration");
  config.add_options()
    ("database.mysql.host", po::value<std::string>(), "mysql server host")
    ("database.mysql.user", po::value<std::string>(), "mysql user")
    ("database.mysql.password", po::value<std::string>(), "mysql password")
    ("database.mysql.database", po::value<std::string>(), "mysql database")
    ("notify.mail.availability_body", po::value<std::string>(), 
     "Default e-mail body for availability notifications")
    ("notify.mail.assignment_body", po::value<std::string>(), 
     "Default e-mail body for assignment notifications");
  po::variables_map vm;

  std::ifstream ifs_home(homerc);
  
  bool systemwide_config = false;

  if (!ifs_home)
  {
    systemwide_config = true;
    std::cout << "User config file "<<homerc<<" cannot be accessed. "
  	         "Trying user configuration file "<<etcrc<<std::endl;
    std::ifstream ifs_etc(etcrc);
    if (!ifs_etc)
      throw std::runtime_error("No system-wide or user config file accessible");
    else
    {
      if(!check_file_permissions(etcrc))
        throw std::runtime_error("Wrong permissions to configuration file. "
		               "Only owner can have read permissions");
      po::store(parse_config_file(ifs_etc, config), vm);
    }
  }
  else
  {
    if(!check_file_permissions(homerc))
      throw std::runtime_error("Wrong permissions to configuration file. "
		               "Only owner can have read permissions");
    po::store(parse_config_file(ifs_home, config), vm);
  }

  po::notify(vm);

  std::string mysql_host = vm["database.mysql.host"].as<std::string>();
  std::string mysql_user = vm["database.mysql.user"].as<std::string>();
  std::string mysql_passwd = vm["database.mysql.password"].as<std::string>();
  std::string mysql_database = vm["database.mysql.database"].as<std::string>();

  db_connection += "host="+mysql_host+";user="+mysql_user+";password="+
	           mysql_passwd+";set_charset_name=utf8;database="+mysql_database;

  std::string availability_body_file;
  std::string assignment_body_file;
  if(systemwide_config)
  {
    availability_body_file = std::string("/etc/elpla/")+
      vm["notify.mail.availability_body"].as<std::string>();
    assignment_body_file = std::string("/etc/elpla/")+
      vm["notify.mail.assignment_body"].as<std::string>();
  }
  else
  {
    availability_body_file = std::string(homedir)+"/.elpla/"+
      vm["notify.mail.availability_body"].as<std::string>();
    assignment_body_file = std::string(homedir)+"/.elpla/"+
      vm["notify.mail.assignment_body"].as<std::string>();
  }
    
  std::ifstream ifs_availability_body(availability_body_file);
  availability_text = std::string((std::istreambuf_iterator<char>(ifs_availability_body)),
		                  std::istreambuf_iterator<char>());
  
  std::ifstream ifs_assignment_body(assignment_body_file);
  assignment_text = std::string((std::istreambuf_iterator<char>(ifs_assignment_body)),
	                        std::istreambuf_iterator<char>());
}

bool ed_config::check_file_permissions(const std::string& file)
{
  struct stat statbuf;

  if(stat(file.c_str(), &statbuf))
    return false;

  mode_t perm = statbuf.st_mode;
  if((perm & S_IRGRP) || (perm & S_IROTH))
    return false;

  return true;
}
