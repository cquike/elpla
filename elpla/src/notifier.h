#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <vector>
#include <future>
#include <deque>
#include <thread>
#include <curses.h>
#include <vmime/vmime.hpp>
#include "eltern.h"
#include "shift_list.h"
#include "email_type.h"


class notifier
{

  public:
  
  notifier(const ed_config& config, bool dry);

  ~notifier();

  bool enqueue_email(email_type type, 
                     boost::gregorian::date date, 
                     const eltern& eltern, 
                     const std::string& body_text,
                     const std::string& subject,
                     const shift_list& shifts = shift_list());

  void start_sending();

  void print_status();

  void print_final_status();

  bool queue_empty();

  private:

  bool send_smtp(std::vector<std::string>& email_to, 
                 std::vector<std::string>& email_cc, 
                 std::string& subject, 
                 std::string& text,
                 std::vector<std::string>& attachments);

  std::string create_icalendar_file(const shift& shift);

  void send_in_background();

  std::string& get_temp_dir();

  std::deque<std::packaged_task<bool()>> m_sending_email_tasks;

  std::vector<std::future<bool>> m_sending_email_futures;
  std::vector<bool> m_sending_email_results;
  std::vector<std::string> m_status_messages;
  std::shared_future<void> m_sending_bkg_future;
  std::vector<std::string> m_error_messages;

  std::string m_temp_dir;
  int m_cal_idx;
  bool m_dry;
  const ed_config& m_config;
};

class emptyCertificateVerifier : public vmime::security::cert::defaultCertificateVerifier
{
  public:
  void verify(vmime::ref <vmime::security::cert::certificateChain> chain);
};

#endif
