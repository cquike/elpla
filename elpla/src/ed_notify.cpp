#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include "ed_config.h"
#include "parent_list.h"
#include "shift_list.h"
#include "notifier.h"
#include "email_type.h"

//TODO: i18n

void notify_parents(const parent_list& parents, const shift_list& shift_assignations, email_type etype, 
		    std::string& email_body_template, const std::string& subject, bool dry, const ed_config& config);

int main(int argc, char *argv[])
{
  try
  {
    ed_config config;
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("setavailtext,v", boost::program_options::value<std::string>(), 
             "set email content for filling availability from file") 
            ("setassigntext,t", boost::program_options::value<std::string>(),
             "set email content for notifying assignations from file")
            ("subject,s", boost::program_options::value<std::string>(),
             "set email subject")
            ("eid,e", boost::program_options::value<std::vector<int> >(),
             "send emails only to some eltern id")
            ("sendavail,l", 
             "send email for filling availability")
            ("sendassign,n", 
             "send email for notifying assignations")
            ("dry,d",
             "just print emails to standard output, do not send them")
            ;

    boost::program_options::variables_map vm;        
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);    

    if(vm.count("help")) 
    {
      std::cout << desc << "\n";
      return 0;
    }

    //Read options
    email_type etype;
    if(vm.count("setavailtext"))
    {
      std::ifstream ifs(vm["setavailtext"].as<std::string>());
      config.availability_text.assign((std::istreambuf_iterator<char>(ifs) ),
                                    (std::istreambuf_iterator<char>()));
    }
    if(vm.count("setassigntext"))
    {
      std::ifstream ifs(vm["setassigntext"].as<std::string>());
      config.assignment_text.assign((std::istreambuf_iterator<char>(ifs) ),
                                    (std::istreambuf_iterator<char>()));
    }
    if(vm.count("sendavail"))
      etype = email_avail;
    else if(vm.count("sendassign"))
      etype = email_assign;
    else //TODO: Disallow setting both
    {
      std::cout<<"At least one option --sendavail or --sendassign must be set"<<std::endl;
      return 1;
    }


    //Read everything from database
    parent_list parents;
    shift_list shift_assignations;
    from_db(parents, config.db_connection);
    from_db(shift_assignations, config.db_connection);

    if(vm.count("eid"))
    {
      std::vector<int> eids = vm["eid"].as<std::vector<int>>();
      for( auto it = parents.begin(); it != parents.end(); )
      {
        if(std::find(eids.begin(), eids.end(), it->first) != eids.end() )
        {
          ++it;
        }
        else
        {
          it = parents.erase(it);
        }
      }
    }

    //Send the emails
    std::string body_text_template;
    if(etype==email_avail)
      body_text_template=config.availability_text;
    else if(etype==email_assign)
      body_text_template=config.assignment_text;
    std::string subject = vm.count("subject") ? vm["subject"].as<std::string>() : "";
    notify_parents(parents, shift_assignations, etype, body_text_template, subject, vm.count("dry")>0, config);
  }
  catch(std::exception& e) 
  {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) 
  {
    std::cerr << "Exception of unknown type!\n";
  }
}

void notify_parents(const parent_list& parents, const shift_list& shift_assignations, email_type etype, std::string& email_body_template, const std::string& subject, bool dry, const ed_config& config)
{
  //Get the date
  boost::gregorian::date current_month = boost::gregorian::day_clock::local_day();
  boost::gregorian::date target_month;
  if(etype == email_avail)
    target_month = current_month + boost::gregorian::weeks(4); //TODO: base this on a database of closed/open months
  else
    target_month = current_month + boost::gregorian::weeks(2);
  target_month = target_month.end_of_month();
  boost::gregorian::date start_next_month(target_month.year(), target_month.month(), 1);
  boost::gregorian::date_period month_period(start_next_month, target_month);

  //Initialize notifier
  notifier notifier(config, dry);

  //Loop on parents
  for(auto& parent : parents)
  { 
    if(parent.second.is_member_at(target_month))
    {
      shift_list this_parent_shifts;
      auto select_shifts = [&parent, &this_parent_shifts, &month_period](const shift& this_shift) 
                           {
                             if(this_shift.parent_id() == parent.first && month_period.contains(this_shift.when()))
                               this_parent_shifts.push_back(this_shift);
                             return;
                           }; 
      std::for_each(shift_assignations.begin(), shift_assignations.end(), select_shifts);

      std::cout<<" Queueing email to "<<parent.second.name_kid()<<std::endl;
      notifier.enqueue_email(etype, target_month, parent.second, email_body_template, subject, this_parent_shifts);
    } 
 }

  notifier.start_sending();

  while(!notifier.queue_empty())
  {
    notifier.print_status();
    sleep(1);
  }
  notifier.print_status();
  notifier.print_final_status();
}
