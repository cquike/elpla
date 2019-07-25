#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <string>
#include "ed_config.h"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

template<class T>
const T& ed_config::get(const std::string& key) const {
  auto opt = vm[key];
  if (opt.empty()) {
    std::cout << "Missing configuration variable " << key;
    auto const spec = config.find_nothrow(key, false);
    if (spec) {
      std::cout << ": " << spec->description();
    }
    std::cout << std::endl;
    throw std::runtime_error("No key " + key + " in configuration.");
  }

  return opt.as<T>();
}

ed_config::ed_config() :
  config("Configuration"),
  availability_text(),
  assignment_text(),
  db_connection("mysql:")
{

  std::string etcrc = "/etc/elpla/elpla.rc";
  const char *homedir;

  if ((homedir = getenv("HOME")) == NULL) 
    throw std::runtime_error("Cannot get home dir");

  std::string homerc = std::string(homedir)+"/.elpla/elpla.rc";

  config.add_options()
    ("database.mysql.host", po::value<std::string>(), "mysql server host")
    ("database.mysql.user", po::value<std::string>(), "mysql user")
    ("database.mysql.password", po::value<std::string>(), "mysql password")
    ("database.mysql.database", po::value<std::string>(), "mysql database")
    ("notify.mail.email_notifier", po::value<std::string>(), 
     "email address which sends notifications")
    ("notify.mail.smtp_server", po::value<std::string>(), 
     "SMTP server for the email account that sends notifications")
    ("notify.mail.smtp_user", po::value<std::string>(), 
     "SMTP user authentication for the email account that sends notifications")
    ("notify.mail.smtp_password", po::value<std::string>(), 
     "SMTP password authentication for the email account that sends notifications")
    ("notify.mail.availability_body", po::value<std::string>(), 
     "Default e-mail body for availability notifications")
    ("notify.mail.assignment_body", po::value<std::string>(), 
     "Default e-mail body for assignment notifications");

  po::store(po::parse_environment(config, [] (std::string envstr){
    if (envstr.size() < 3) {
      return std::string();
    };
    if (envstr.substr(0, 3) != "ED_") {
      return std::string();
    };
    std::string data = envstr.substr(3);
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c){ return (c == '_') ? '.' : std::tolower(c); });
    return data;
  }), vm);

  std::ifstream ifs_home(homerc);
  
  bool systemwide_config = false;
  bool home_config = false;

  if (!ifs_home)
  {
    std::cout << "User config file "<<homerc<<" cannot be accessed. "
  	         "Trying user configuration file "<<etcrc<<std::endl;
    std::ifstream ifs_etc(etcrc);

    if (!ifs_etc)
    {
      std::cout << "Config file "<< etcrc <<" cannot be accessed." <<std::endl;
    }
    else
    {
      systemwide_config = true;
      po::store(parse_config_file(ifs_etc, config), vm);
    }
  }
  else
  {
    if(!check_file_permissions(homerc))
      throw std::runtime_error("Wrong permissions to configuration file. "
                               "Only owner can have read permissions");
    home_config = true;
    po::store(parse_config_file(ifs_home, config), vm);
  }

  po::notify(vm);

  std::string mysql_host = get<std::string>("database.mysql.host");
  std::string mysql_user = get<std::string>("database.mysql.user");
  std::string mysql_passwd = get<std::string>("database.mysql.password");
  std::string mysql_database = get<std::string>("database.mysql.database");

  db_connection += "host="+mysql_host+";user="+mysql_user+";password="+
	           mysql_passwd+";set_charset_name=utf8;database="+mysql_database;

  std::string availability_body_file;
  std::string assignment_body_file;
  std::string prefix;

  if(systemwide_config)
  {
    prefix = std::string("/etc/elpla/");
  }
  else if (home_config)
  {
    prefix = std::string(homedir)+"/.elpla/";
  }

  availability_body_file = prefix + get<std::string>("notify.mail.availability_body");
  assignment_body_file = prefix + get<std::string>("notify.mail.assignment_body");
    
  std::ifstream ifs_availability_body(availability_body_file);
  if (!ifs_availability_body) {
    throw std::runtime_error("Cannot open file for reading " + availability_body_file);
  }
  availability_text = std::string((std::istreambuf_iterator<char>(ifs_availability_body)),
		                  std::istreambuf_iterator<char>());

  std::ifstream ifs_assignment_body(assignment_body_file);
  if (!ifs_assignment_body) {
    throw std::runtime_error("Cannot open file for reading " + assignment_body_file);
  }
  assignment_text = std::string((std::istreambuf_iterator<char>(ifs_assignment_body)),
	                        std::istreambuf_iterator<char>());

  email_notifier = get<std::string>("notify.mail.email_notifier");

  smtp_server = get<std::string>("notify.mail.smtp_server");

  smtp_user= get<std::string>("notify.mail.smtp_user");

  smtp_password= get<std::string>("notify.mail.smtp_password");
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
