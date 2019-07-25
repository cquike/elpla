#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WMessageBox>
#include <Wt/WTable>
#include <Wt/WHBoxLayout>
#include <Wt/WString>
#include <Wt/WTemplate>
#include <Wt/WPanel>
#include <Wt/WBootstrapTheme>
#include <Wt/WDateEdit>
#include <Wt/WDate>
#include <Wt/WCalendar>
#include "ed_config.h"
#include "eltern.h"
#include "parent_list.h"
#include "befreiung.h"
#include "waiver_list.h"
#include "closing_days.h"

// c++0x only, for std::bind
// #include <functional>

class PresenceWidget : public Wt::WContainerWidget
{
public:

  PresenceWidget(parent_list& parents, waiver_list& waivers, //TODO: It has to be with kids, rather than parents
                 closing_day_list& closing_days,
                 std::string& locale, 
                 Wt::WContainerWidget *parent_widget =0); 
  ~PresenceWidget();
  void update();
  void change_month(bool increment);

private:
  parent_list m_parents;
  waiver_list m_waivers;
  closing_day_list m_closing_days;
  std::string m_locale;
  boost::gregorian::date m_start_month;
};

PresenceWidget::PresenceWidget(parent_list& parents, waiver_list& waivers,
                               closing_day_list& closing_days,
                               std::string& locale,
                               Wt::WContainerWidget *parent_widget) :
  WContainerWidget(parent_widget), m_parents(parents), m_waivers(waivers),
  m_closing_days(closing_days), m_locale(locale)
{
  //Get the month
  boost::gregorian::date current_month = boost::gregorian::day_clock::local_day();
  boost::gregorian::date next_month;
  next_month = current_month.end_of_month();
  boost::gregorian::day_iterator ditr(next_month);
  m_start_month = *(++ditr);

  setStyleClass("presence");
  update();
}

PresenceWidget::~PresenceWidget()
{
}

void PresenceWidget::update()
{
  clear();

  //std::locale::global(std::locale("de_DE.utf8"));
  std::string title = "Anzahl der Kinder beim Mafalda";
  Wt::WText *title_wt = new Wt::WText("<h2>"+title+"</h2>");
  addWidget(title_wt);

  Wt::WText *explanation = new Wt::WText("<p>Diese Seite zeigt wie viele Kinder besuchen Mafalda am einem bestimte Tag.</p> ");
  addWidget(explanation);

  Wt::WContainerWidget *month_container = new Wt::WContainerWidget();
  month_container->resize(500, 50);
  addWidget(month_container);
  Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
  month_container->setLayout(hbox);
  
  Wt::WPushButton *previous_month_button = new Wt::WPushButton("<<");
  hbox->addWidget(previous_month_button);
  previous_month_button->mouseWentDown().connect(std::bind(&PresenceWidget::change_month, this, false));

  boost::gregorian::date_facet* f = new boost::gregorian::date_facet("%x");
  f->month_format("%B");
  //std::locale loc = std::locale(std::locale(m_locale), f); //TODO: Does not work, because locale is something like es-ES, which std::locale doesn't understand
  std::locale loc = std::locale(std::locale("de_DE.utf8"), f);
  std::ostringstream ss_month;
  ss_month.imbue(loc);
  ss_month << m_start_month.month();

  Wt::WText *month_text = new Wt::WText(Wt::WString("<h3>"+ss_month.str()+"</h3>", Wt::UTF8));
  month_text->setTextAlignment(Wt::AlignCenter);
  hbox->addWidget(month_text);

  Wt::WPushButton *next_month_button = new Wt::WPushButton(">>");
  hbox->addWidget(next_month_button);
  next_month_button->mouseWentDown().connect(std::bind(&PresenceWidget::change_month, this, true)); 

  addWidget(new Wt::WBreak());

  Wt::WTable *presence_table = new Wt::WTable(this);
  boost::gregorian::day_iterator ditr(m_start_month);
  new Wt::WText("Datum", presence_table->elementAt(0, 0));
  auto col_title = new Wt::WText("Gesamte Kinder", presence_table->elementAt(0, 1));
  col_title->setStyleClass("verticalText");
  col_title->setInline(false);
  col_title = new Wt::WText("Teilzeit Kinder", presence_table->elementAt(0, 2));
  col_title->setStyleClass("verticalText");
  col_title->setInline(false);
  col_title = new Wt::WText("Vollzeit Kinder", presence_table->elementAt(0, 3));
  col_title->setStyleClass("verticalText");
  col_title->setInline(false);

  //Get a map with kids name 
  std::map<std::string, int> kids_map;
  for (auto& parent : m_parents)
    kids_map.insert(std::make_pair(parent.second.name_kid(), parent.first));

  int kid_column = 4;
  for (auto& kid : kids_map)
  {
    auto parent = m_parents[kid.second];
    if(parent.is_member_at(m_start_month) || parent.is_member_at(m_start_month.end_of_month()) )
    {
      auto parents_text = new Wt::WText(Wt::WString(parent.name_kid(), Wt::UTF8),
                    presence_table->elementAt(0, kid_column));
      parents_text->setStyleClass("verticalText");
      parents_text->setInline(false);
      kid_column++;
    }
  }

  for (int row = 0; ditr <= m_start_month.end_of_month(); ++ditr, ++row) 
  {
      std::ostringstream ss_day;
      ss_day.imbue(loc);
      ss_day << *ditr;
      new Wt::WText(ss_day.str(), presence_table->elementAt(row+1, 0));
  }
  presence_table->setHeaderCount(1);

  ditr = m_start_month;
  for (int i = 0; ditr <= m_start_month.end_of_month(); ++ditr, ++i) 
  {
    int total_kids = 0;
    int total_full_time = 0;
    int total_part_time = 0;
    if(ditr->day_of_week() == boost::gregorian::Saturday ||
       ditr->day_of_week() == boost::gregorian::Sunday ||
       m_closing_days.is_closed(*ditr))
    {
      presence_table->rowAt(i+1)->setStyleClass("dayClosed");
      continue;
    }
    int kid_column = 4;
    for (auto& kid : kids_map)
    {
      auto parent = m_parents[kid.second];
      if(parent.is_member_at(m_start_month) || parent.is_member_at(m_start_month.end_of_month()) )
      {
        auto parent_id = kid.second;
        bool present = true;
        if(!parent.is_member_at(*ditr))
          present = false;
        for (auto& waiver_id : m_waivers)
        {
          auto& waiver = waiver_id.second;
          if (waiver.parent_id() == parent_id && waiver.period().contains(*ditr) &&
              waiver.cause() == "Urlaub") 
            present = false;
        }
        if(present)
        {
          total_kids++;
          if (parent.visit_hours() == "Parttime")
          {
            total_part_time++;
            presence_table->elementAt(i+1, kid_column)->setStyleClass("kidPresentPartTime");
          }
          if (parent.visit_hours() == "Fulltime")
          {
            total_full_time++;
            presence_table->elementAt(i+1, kid_column)->setStyleClass("kidPresentFullTime");
          }
        }
        else
          presence_table->elementAt(i+1, kid_column)->setStyleClass("kidNotPresent");
        kid_column++;
      }
    }
    new Wt::WText(std::to_string(total_kids), presence_table->elementAt(i+1, 1));
    new Wt::WText(std::to_string(total_part_time), presence_table->elementAt(i+1, 2));
    new Wt::WText(std::to_string(total_full_time), presence_table->elementAt(i+1, 3));
  }

  addWidget(new Wt::WBreak());


}

void PresenceWidget::change_month(bool increment)
{
  boost::gregorian::month_iterator ditr(m_start_month);
  if(increment)
    ++ditr;
  else
    --ditr;
  m_start_month = *ditr;
  update();
}

Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  Wt::WApplication *app = new Wt::WApplication(env);

  //Read Database
  ed_config config;
  parent_list parents;
  waiver_list befreiungen(config.db_connection);
  closing_day_list closing_days(config.db_connection);
  from_db(parents, config.db_connection);
  befreiungen.from_db();
  closing_days.from_db();


  //Get browser locale
  auto browser_locale = app->locale().name();

  app->setTitle("Kinder besuchen Mafalda");
  PresenceWidget * presence = new PresenceWidget(parents, befreiungen, closing_days, browser_locale, app->root());

  //Theme
  Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(app);
  bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
  bootstrapTheme->setResponsive(true);
  app->setTheme(bootstrapTheme);
  app->useStyleSheet("css/ed.css");

  return app;
}

int main(int argc, char **argv)
{
  /*
   * Your main method may set up some shared resources, but should then
   * start the server application (FastCGI or httpd) that starts listening
   * for requests, and handles all of the application life cycles.
   *
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */

 
  return Wt::WRun(argc, argv, &createApplication);
}

