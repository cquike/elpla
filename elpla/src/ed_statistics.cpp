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
#include "shift_list.h"

// c++0x only, for std::bind
// #include <functional>

class StatisticsWidget : public Wt::WContainerWidget
{
public:

  StatisticsWidget(parent_list& parents, waiver_list& waivers, //TODO: It has to be with kids, rather than parents
                   shift_list& shifts, closing_day_list& closing_days,
                   std::string& locale, 
                   Wt::WContainerWidget *parent_widget =0); 
  ~StatisticsWidget();
  void update();

private:
  parent_list m_parents;
  waiver_list m_waivers;
  closing_day_list m_closing_days;
  shift_list m_shifts;
  std::string m_locale;
  boost::gregorian::date_period m_stats_period;
  boost::gregorian::date m_today;
};

StatisticsWidget::StatisticsWidget(parent_list& parents, waiver_list& waivers,
                                   shift_list& shifts, closing_day_list& closing_days,
                                   std::string& locale,
                                   Wt::WContainerWidget *parent_widget) :
  WContainerWidget(parent_widget), m_parents(parents), m_waivers(waivers),
  m_shifts(shifts), m_closing_days(closing_days), m_locale(locale),
  m_stats_period(boost::gregorian::date(boost::gregorian::pos_infin), 
                 boost::gregorian::date(boost::gregorian::pos_infin))
{
  //Get the month
  m_today = boost::gregorian::day_clock::local_day();
  boost::gregorian::date_duration one_year(365);
  auto one_year_ago = m_today.end_of_month() - one_year;
  auto end_of_month = m_today.end_of_month();
  m_stats_period = boost::gregorian::date_period(one_year_ago, end_of_month);
//                       boost::gregorian::date(boost::gregorian::pos_infin));
//                                 m_today + boost::gregorian::date_duration(1));

  setStyleClass("statistics");
  update();
}

StatisticsWidget::~StatisticsWidget()
{
}

void StatisticsWidget::update()
{
  clear();

  //std::locale::global(std::locale("de_DE.utf8"));
  std::string title = "Elterndienst Statistik";
  Wt::WText *title_wt = new Wt::WText("<h2>"+title+"</h2>");
  addWidget(title_wt);

  Wt::WText *explanation = new Wt::WText("<p>Diese Seite zeigt wie viele Bereitschftselterndienste jeder Elternteil erledigt.</p> ");
  addWidget(explanation);

  addWidget(new Wt::WBreak());

  Wt::WTable *statistics_table = new Wt::WTable(this);
  new Wt::WText("", statistics_table->elementAt(0, 0));
  new Wt::WText("Gesamte Bereitschatfs ED vormittags ( / Jahr)", statistics_table->elementAt(1, 0));
  new Wt::WText("Gesamte Bereitschatfs ED nachmittags ( / Jahr)", statistics_table->elementAt(2, 0));
  new Wt::WText("Gesamte verpflichtende ED vormittags ( / Jahr)", statistics_table->elementAt(3, 0));
  new Wt::WText("Gesamte Bverpflichtende ED nachmittags ( / Jahr)", statistics_table->elementAt(4, 0));
  new Wt::WText("Anzahl Tage ohne Befreiung im Jahr", statistics_table->elementAt(5, 0));
  statistics_table->setHeaderCount(1);

  int parent_column = 1;
  for (auto& parent : m_parents)
  {
    if(parent.second.is_member_at(m_today))
    {
      auto parent_id = parent.first;
      int available_days = parent.second.get_available_days(m_stats_period, 
                     m_waivers, m_closing_days);
      int total_morning_shifts = 
          m_shifts.count_assigned(m_stats_period, parent_id, "v");
      int total_afternoon_shifts = 
          m_shifts.count_assigned(m_stats_period, parent_id, "n");
      int total_morning_shifts_mandatory = 
          m_shifts.count_assigned_mandatory(m_stats_period, parent_id, "v");
      int total_afternoon_shifts_mandatory = 
          m_shifts.count_assigned_mandatory(m_stats_period, parent_id, "n");
      auto parents_text = new Wt::WText(Wt::WString(parent.second.name_mother() + " und " + parent.second.name_father(), Wt::UTF8), 
                    statistics_table->elementAt(0, parent_column));
      parents_text->setStyleClass("verticalText");
      parents_text->setInline(false);
      if(available_days != 0)
      {
        double fraction_year = 365. / available_days; 
        std::ostringstream ss_number;
        ss_number << std::fixed << std::setprecision(2);
        ss_number << fraction_year * total_morning_shifts;
        new Wt::WText(std::to_string(total_morning_shifts) + " (" + ss_number.str() + ")", 
                      statistics_table->elementAt(1, parent_column));
        ss_number.str("");
        ss_number << fraction_year * total_afternoon_shifts;
        new Wt::WText(std::to_string(total_afternoon_shifts) + " (" + ss_number.str() + ")", 
                      statistics_table->elementAt(2, parent_column));
        ss_number.str("");
        ss_number << fraction_year * total_morning_shifts_mandatory;
        new Wt::WText(std::to_string(total_morning_shifts_mandatory) + " (" + ss_number.str() + ")", 
                      statistics_table->elementAt(3, parent_column));
        ss_number.str("");
        ss_number << fraction_year * total_afternoon_shifts_mandatory;
        new Wt::WText(std::to_string(total_afternoon_shifts_mandatory) + " (" + ss_number.str() + ")", 
                      statistics_table->elementAt(4, parent_column));
        new Wt::WText(std::to_string(available_days), 
                      statistics_table->elementAt(5, parent_column));
      }
      else
      {
        new Wt::WText(std::to_string(total_morning_shifts) + " (N/A)", 
                      statistics_table->elementAt(1, parent_column));
        new Wt::WText(std::to_string(total_afternoon_shifts) + " (N/A)", 
                      statistics_table->elementAt(2, parent_column));
        new Wt::WText(std::to_string(total_morning_shifts_mandatory) + "( N/A)", 
                      statistics_table->elementAt(3, parent_column));
        new Wt::WText(std::to_string(total_afternoon_shifts_mandatory) + " (N/A)", 
                      statistics_table->elementAt(4, parent_column));
        new Wt::WText("0", statistics_table->elementAt(5, parent_column));
      }
      
      parent_column++;
    }
  }

  addWidget(new Wt::WBreak());


}

Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  Wt::WApplication *app = new Wt::WApplication(env);

  //Read Database
  ed_config config;
  parent_list parents;
  shift_list shifts;
  closing_day_list closing_days(config.db_connection);
  waiver_list befreiungen(config.db_connection);
  from_db(parents, config.db_connection);
  from_db(shifts, config.db_connection);
  befreiungen.from_db();
  closing_days.from_db();

  //Get browser locale
  auto browser_locale = app->locale().name();

  app->setTitle("Elterndienst Statistiks");
  StatisticsWidget * presence = new StatisticsWidget(parents, befreiungen, 
                                           shifts, closing_days,
                                           browser_locale, app->root());

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

