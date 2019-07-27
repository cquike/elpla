#include <fstream>
#include <iostream>
#include <algorithm>
#include <regex>
#include <vmime/vmime.hpp>
#include <vmime/platforms/posix/posixHandler.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "ed_config.h"
#include "notifier.h"


notifier::notifier(const ed_config& config, bool dry) :
  m_cal_idx(0),
  m_dry(dry),
  m_config(config)
{
  vmime::platform::setHandler<vmime::platforms::posix::posixHandler>();
}

notifier::~notifier()
{
  if(!m_sending_email_tasks.empty() && m_sending_email_futures.empty())
  {
    std::cerr<<"Aborting pending emails"<<std::endl;
    m_sending_email_tasks.clear();
  }
  else
  {
    m_sending_bkg_future.get();
  }
}

bool notifier::enqueue_email
(email_type type,
 boost::gregorian::date title_date,
 boost::gregorian::date opened_deadline,
 const eltern& eltern, 
 const std::string& body_text,
 const std::string& subject,
 const shift_list& shifts)
{
  std::locale::global(std::locale("de_DE.utf8"));
  std::string message("Sending email to ");
  message+=eltern.email_mother()+", "+eltern.email_father()+"...";
  m_status_messages.push_back(message);
  std::vector<std::string> email_to;
  std::vector<std::string> email_cc;
  char month_c[100];
  std::tm date_tm = boost::gregorian::to_tm(title_date);
  std::strftime(month_c, sizeof(month_c)-1, "%B %Y", &date_tm);
  char deadline_c[100];
  std::tm deadline_tm = boost::gregorian::to_tm(opened_deadline);
  std::strftime(deadline_c, sizeof(deadline_c)-1, "%d.%m.%Y", &deadline_tm);
  std::string email_subject;
  if (!subject.empty()) {
    email_subject = subject;
  } else {
    email_subject = std::string("Elterndienste ") + month_c;
  }
  std::string text = body_text; 
  text = std::regex_replace(text, std::regex("\\$Name_Mutter"), eltern.name_mother());
  text = std::regex_replace(text, std::regex("\\$Name_Vater"), eltern.name_father());
  text = std::regex_replace(text, std::regex("\\$Passwort"), eltern.password());
  text = std::regex_replace(text, std::regex("\\$SystemUrl"), m_config.system_url);
  text = std::regex_replace(text, std::regex("\\$SystemMainteners"), m_config.system_mainteners);
  text = std::regex_replace(text, std::regex("\\$OpenedDeadline"), deadline_c);
  email_to.push_back(eltern.email_mother());
  email_to.push_back(eltern.email_father());
  email_cc.push_back(m_config.email_notifier);
  std::vector<std::string> attachments;

  if(type == email_assign)
    for(auto& shift : shifts)
      attachments.push_back(create_icalendar_file(shift));

  if (m_dry) {
    std::cout << "Eid: "
                    << eltern.id() << std::endl
              << "From: '"
                    << m_config.email_notifier_name << "' <" << m_config.email_notifier << ">"
                    << std::endl
              << "To: "
                    << "'" << eltern.name_mother() << "' <" << eltern.email_mother() << ">"
                    << ", "
                    << "'" << eltern.name_father() << "' <" << eltern.email_father() << ">"
                    << std::endl
              << "Subject: "
                    << email_subject
                    << std::endl
              << text
              << std::endl << std::endl;

    size_t attachments_size = attachments.size();
    for (size_t i = 0; i < attachments_size; ++i) {
      std::cout << "Attachment [" << i << "]: " << attachments[i] << std::endl;
    }
    std::cout << std::endl;
    return true;
  }

  m_sending_email_tasks.push_back
    (std::packaged_task<bool()>
       (std::bind(&notifier::send_smtp, this, email_to, email_cc, email_subject, text, attachments)));
  m_sending_email_futures.push_back(m_sending_email_tasks.back().get_future());
  return true;
}

bool notifier::queue_empty()
{
  for(auto& this_future : m_sending_email_futures)
    if(this_future.valid())
      if(this_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
        return false;
  return true;
}

void notifier::start_sending()
{
  m_sending_bkg_future = std::async(std::launch::async, 
                                    std::bind(&notifier::send_in_background, this));
}

void notifier::send_in_background()
{
  m_sending_email_results.resize(m_sending_email_tasks.size());
  while ( ! m_sending_email_tasks.empty() )
  {
    std::packaged_task<bool()> this_task = std::move(m_sending_email_tasks.front());
    m_sending_email_tasks.pop_front();
    this_task();
  }
}

void notifier::print_status()
{
//  (void) initscr();      /* initialize the curses library */
//  start_color();
//  init_pair(1, COLOR_BLACK, COLOR_RED);
//  init_pair(2, COLOR_BLACK, COLOR_GREEN);
  if(m_sending_email_futures.empty())
  {
    std::cout<<"Emails are not yet being sent"<<std::endl;
    return;
  }
  int i=0;
  for(auto& message : m_status_messages)
  {
    std::string status;
    if(m_sending_email_futures[i].valid())
    {
      if(m_sending_email_futures[i].wait_for(std::chrono::seconds(0)) == std::future_status::ready)
      {
        bool result = m_sending_email_futures[i].get();
        if(result)
          status="OK";
        else
          status="FAIL";
          m_sending_email_results[i]=result;
      }
      else
        status="";
    }
    else
    {
        if(m_sending_email_results[i])
          status="OK";
        else
          status="FAIL";
    }

    std::cout<<message;
    if(status=="OK")
      std::cout<<"\033[1;32m"<<status<<"\033[0m"<<std::endl;
    else
      std::cout<<"\033[1;31m"<<status<<"\033[0m"<<std::endl;
    i++;
  }
  std::cout<<m_sending_email_tasks.size()<<" emails still in the queue     "<<std::endl;
    std::cout << "\x1b[A";
  for(auto& message : m_status_messages)
    std::cout << "\x1b[A";
}

void notifier::print_final_status()
{
  for(auto& message : m_status_messages)
    std::cout << "\x1b[B";
  std::cout << "\x1b[B";
  std::cout<<std::count(m_sending_email_results.begin(), m_sending_email_results.end(), true) <<" emails successfully sent"<<std::endl;
  std::cout<<std::count(m_sending_email_results.begin(), m_sending_email_results.end(), false)  <<" emails failed to sent"<<std::endl;
  if(!m_error_messages.empty())
  {
    std::cout<<"Error messages"<<std::endl; 
    for (auto& message : m_error_messages)
      std::cout<<message<<std::endl;
  }
}

static std::ostream& operator<<(std::ostream& os, const vmime::exception& e) {
  os << "vmime::exceptions::" << e.name() << "{what='" << e.what() << "'";
  // More information for special exceptions
  if (dynamic_cast <const vmime::exceptions::command_error*>(&e)) {
    const auto& ce = dynamic_cast <const vmime::exceptions::command_error&>(e);
    os << ",command='" << ce.command() << "'";
    os << ",response='" << ce.response() << "'";
  }
  if (dynamic_cast <const vmime::exceptions::invalid_response*>(&e)) {
    const auto& ir = dynamic_cast <const vmime::exceptions::invalid_response&>(e);
    os << ",response='" << ir.response() << "'";
  }
  if (dynamic_cast <const vmime::exceptions::connection_greeting_error*>(&e)) {
    const auto& cge = dynamic_cast <const vmime::exceptions::connection_greeting_error&>(e);
    os << ",response='" << cge.response() << "'";
  }
  if (dynamic_cast <const vmime::exceptions::authentication_error*>(&e)) {
    const auto& ae = dynamic_cast <const vmime::exceptions::authentication_error&>(e);
    os << ",response='" << ae.response() << "'";
  }
  if (dynamic_cast <const vmime::exceptions::filesystem_exception*>(&e)) {
    const auto& fe = dynamic_cast <const vmime::exceptions::filesystem_exception&>(e);
    os << ",path='" << vmime::platform::getHandler()->getFileSystemFactory()->pathToString(fe.path()) << "'";
  }
  if (e.other()) {
    os << ",other=" << *e.other();
  }
  os << "}";
  return os;
}

bool notifier::send_smtp(std::vector<std::string>& email_to, 
                         std::vector<std::string>& email_cc, 
                         std::string& subject, 
                         std::string& text, 
                         std::vector<std::string>& attachments)
{
  try
  {
    vmime::messageBuilder mb;

    mb.setExpeditor(vmime::mailbox(vmime::text(m_config.email_notifier_name),
                                   m_config.email_notifier));

    vmime::addressList to;
    for(auto& address_to : email_to)
      to.appendAddress(vmime::create <vmime::mailbox>(address_to));
    mb.setRecipients(to);

    vmime::addressList bcc;
    for(auto& address_bcc : email_cc)
      bcc.appendAddress(vmime::create <vmime::mailbox>(address_bcc));
    mb.setBlindCopyRecipients(bcc);

    mb.setSubject(vmime::text(subject));

    // Message body
    mb.getTextPart()->setCharset(vmime::charsets::UTF_8);
    mb.getTextPart()->setText(vmime::create <vmime::stringContentHandler>(text));

    //Attachments
    for(auto& attach_file :attachments)
    {
      vmime::ref <vmime::fileAttachment> attach_vmime = vmime::create <vmime::fileAttachment>
      (
        attach_file,
        vmime::mediaType("text/calendar"), 
        vmime::text("Elterndienst")
      );
      mb.attach(attach_vmime);
    }

    // Construction
    vmime::ref <vmime::message> msg = mb.construct();

    //Session
    vmime::ref <vmime::net::session> session = vmime::create< vmime::net::session >();
    vmime::ref <vmime::security::cert::defaultCertificateVerifier> vrf = 
      vmime::create <emptyCertificateVerifier>();
    vmime::utility::url url(m_config.smtp_server);
    vmime::ref <vmime::net::transport> tr = session->getTransport(url);
    tr->setProperty("connection.tls", true);
    tr->setProperty("auth.username", m_config.smtp_user);
    tr->setProperty("auth.password", m_config.smtp_password);
    tr->setProperty("options.need-authentication", true);
    tr->setCertificateVerifier(vrf);
    tr->connect();
    tr->send(msg);
    tr->disconnect();

    sleep(100);
  }
  catch (vmime::exception& e)
  {
    std::ostringstream error_msg;
    error_msg << e;
    m_error_messages.push_back(error_msg.str());
    sleep(100);
    return false;
  }
  catch (std::exception& e)
  {
    m_error_messages.push_back(e.what());
    sleep(100);
    return false;
  }

  return true;
}

std::string notifier::create_icalendar_file(const shift& shift)
{
  std::string icalendar_template =  
std::string("BEGIN:VCALENDAR\n"
"VERSION:2.0\n"
"BEGIN:VEVENT\n"
"ORGANIZER;CN=\"Mafalda Elterndienst\":MAILTO:")+m_config.smtp_user+"\n"
"DTSTAMP:$When_start\n"
"DESCRIPTION: Mafalda Elterndienst $Morning_Afternoon\n"
"SUMMARY: Mafalda Elterndienst $Morning_Afternoon\n"
"DTSTART:$When_start\n"
"DTEND:$When_end\n"
"END:VEVENT\n"
"END:VCALENDAR\n";
 
  bool morning= (shift.day_period() == "v");
  std::string when_start = boost::gregorian::to_iso_string(shift.when());
  std::string when_end = boost::gregorian::to_iso_string(shift.when());
  std::string morning_afternoon;
  if(morning)
  {
    when_start += "T090000";
    when_end += "T130000";
    morning_afternoon = "vormittags";
  }
  else
  {
    when_start += "T150000";
    when_end += "T173000";
    morning_afternoon = "nachmittags";
  }
  auto icalendar_text = icalendar_template; 
  icalendar_text = std::regex_replace(icalendar_text, 
    std::regex("\\$When_start"), when_start);
  icalendar_text = std::regex_replace(icalendar_text, 
    std::regex("\\$When_end"), when_end);
  icalendar_text = std::regex_replace(icalendar_text, 
    std::regex("\\$Morning_Afternoon"), morning_afternoon);

  std::string& temp_dir = get_temp_dir();
  std::string temp_icalendar = temp_dir + "/cal_" + std::to_string(m_cal_idx + 1) + ".ics";
  m_cal_idx++;
  std::ofstream of_ical;
  of_ical.open(temp_icalendar);
  of_ical << icalendar_text;
  of_ical.close();
  //TODO: remove temportal file 

  return std::string(temp_icalendar);
}

std::string& notifier::get_temp_dir()
{
  if(m_temp_dir == "")
  {
    char templ[] = "/tmp/elterndienst_XXXXXX";
    char * new_dir = mkdtemp(templ);
    if(new_dir == NULL)
      throw std::runtime_error("Cannot create temporary directory. errno: " + 
                               std::to_string(errno));
    m_temp_dir = new_dir;
  }
  return m_temp_dir;
}

//TODO: Do proper verification
void emptyCertificateVerifier::verify(vmime::ref <vmime::security::cert::certificateChain> chain)
{
  return;
}

