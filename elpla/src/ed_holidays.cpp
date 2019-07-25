#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WMessageBox>
#include <Wt/WTable>
#include <Wt/WVBoxLayout>
#include <Wt/WString>
#include <Wt/WTemplate>
#include <Wt/WBootstrapTheme>
#include <Wt/WDateEdit>
#include <Wt/WDate>
#include <Wt/WCalendar>
#include "ed_config.h"
#include "eltern.h"
#include "parent_list.h"
#include "befreiung.h"
#include "waiver_list.h"

// c++0x only, for std::bind
// #include <functional>

class HolidaysWidget : public Wt::WContainerWidget
{
public:

  HolidaysWidget(eltern& parent, waiver_list& waivers,
                 std::string& locale, 
                 Wt::WContainerWidget *parent_widget =0); 
  ~HolidaysWidget();
  void update();

  void remove_holidays(int i);
  void add_holidays(const Wt::WDate& from, const Wt::WDate& to);
  void wait_cursor();
private:
  eltern m_parent;
  waiver_list m_waivers;
  std::string m_locale;
};

HolidaysWidget::HolidaysWidget(eltern& parent, waiver_list& waivers,
                               std::string& locale, 
                               Wt::WContainerWidget *parent_widget) :
  WContainerWidget(parent_widget), m_parent(parent), m_waivers(waivers),
  m_locale(locale)
{

  setStyleClass("holidays");
  update();
}

HolidaysWidget::~HolidaysWidget()
{
}

void HolidaysWidget::update()
{
  clear();


  std::string title = "<h2>"+m_parent.name_mother()+" und "+m_parent.name_father()+ " Eltern Urlaub"+"</h2>";
  Wt::WText *title_wt = new Wt::WText(Wt::WString(title, Wt::UTF8));
  addWidget(title_wt);

  Wt::WText *explanation = new Wt::WText("<p>Bitte tragt eure voraussichtlichen Urlaub ein.</p> "
                                         "<p>Derzeit sind folgende Urlaube eingetragen:</p>");
  addWidget(explanation);

  Wt::WTable *holidays_table = new Wt::WTable(this);
  new Wt::WText("Von", holidays_table->elementAt(0, 0));
  new Wt::WText("Bis", holidays_table->elementAt(0, 1));
  holidays_table->setHeaderCount(1);

  boost::gregorian::date_facet* f = new boost::gregorian::date_facet("%x");
  //std::locale loc = std::locale(std::locale(m_locale), f); //TODO: Does not work, because locale is something like es-ES, which std::locale doesn't understand
  std::locale loc = std::locale(std::locale("de_DE.utf8"), f);

  for (auto& waiver_id : m_waivers) 
  {
    auto& waiver = waiver_id.second;
    if (waiver.parent_id() == m_parent.id() &&
        waiver.cause() == "Urlaub") 
    {
      int row = holidays_table->rowCount();
      std::ostringstream ss_beg;
      ss_beg.imbue(loc);
      ss_beg << waiver.period().begin();
      new Wt::WText(ss_beg.str(), holidays_table->elementAt(row, 0));
      std::ostringstream ss_last;
      ss_last.imbue(loc);
      ss_last<< waiver.period().last();
      new Wt::WText(ss_last.str(), holidays_table->elementAt(row, 1));
      Wt::WPushButton *button_remove = 
        new Wt::WPushButton(Wt::WString("Löschen", Wt::UTF8), holidays_table->elementAt(row, 2));
      button_remove->setMargin(5, Wt::Left); 
      button_remove->mouseWentDown().connect(std::bind(&HolidaysWidget::wait_cursor, this)); //TODO: Make sure this is triggered before the next one. mouseWentDown is a hack, since it can be triggered even if no clicked is triggered.
      button_remove->clicked().connect(std::bind(&HolidaysWidget::remove_holidays, this, waiver_id.first));
    }
  }

  addWidget(new Wt::WBreak());

  Wt::WText *new_text = new Wt::WText("<p>Neue Eingabe:</p> ");
  addWidget(new_text);

  addWidget(new Wt::WBreak());

  Wt::WDateEdit *date_edit_from = new Wt::WDateEdit();
  date_edit_from->setDate(Wt::WDate::currentServerDate().addDays(1));
  date_edit_from->setMaximumSize(300,50);
  addWidget(date_edit_from);

  Wt::WDateEdit *date_edit_to = new Wt::WDateEdit();
  date_edit_to->setBottom(date_edit_from->date());
  date_edit_to->setMaximumSize(300, 50);
  addWidget(date_edit_to);

  Wt::WPushButton *button = new Wt::WPushButton("Speichern");
  addWidget(button);

  date_edit_from->changed().connect(std::bind([=] () {
      if (date_edit_from->validate() == Wt::WValidator::Valid)
        date_edit_to->setBottom(date_edit_from->date());   }));

  date_edit_to->changed().connect(std::bind([=] () {
      if (date_edit_from->validate() == Wt::WValidator::Valid) 
        date_edit_from->setTop(date_edit_to->date());      }));

  button->mouseWentDown().connect(std::bind(&HolidaysWidget::wait_cursor, this)); //TODO: Make sure this is triggered before the next one. mouseWentDown is a hack, since it can be triggered even if no clicked is triggered.
  button->clicked().connect(std::bind([=] () {
      if (date_edit_from->text().empty() || date_edit_to->text().empty())
        std::cout<<"Invalid"<<std::endl; //TODO: Popup
      else {
        add_holidays(date_edit_from->date(), date_edit_to->date());
      }
    }));
}

void HolidaysWidget::wait_cursor()
{
  parent()->decorationStyle().setCursor(Wt::WaitCursor);
}

void HolidaysWidget::remove_holidays(int i)
{
  m_waivers.remove(i);
  update();
  parent()->decorationStyle().setCursor(Wt::AutoCursor);
}

void HolidaysWidget::add_holidays(const Wt::WDate& from, const Wt::WDate& to)
{
  auto from_date = 
    boost::gregorian::from_undelimited_string(from.toString(Wt::WString("yyyyMMdd")).toUTF8());
  auto to_date = 
    boost::gregorian::from_undelimited_string(to.toString(Wt::WString("yyyyMMdd")).toUTF8());
  m_waivers.add(m_parent.id(), from_date, to_date, "Urlaub");
  update();
  parent()->decorationStyle().setCursor(Wt::AutoCursor);
}

Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  Wt::WApplication *app = new Wt::WApplication(env);

  std::string password;
  if (!env.getParameterValues("PW").empty())
    password = env.getParameterValues("PW")[0];
  else
  {
    Wt::WMessageBox *messageBox = new Wt::WMessageBox
      ("No user has been provided", "", Wt::Information, Wt::Yes );
    messageBox->setModal(false);
    messageBox->buttonClicked().connect(std::bind([=] () { delete messageBox; }));
    messageBox->show(); //TODO: This does not work
    throw std::runtime_error("No user provided");
  }

  //Read Database
  ed_config config;
  parent_list parents;
  waiver_list befreiungen(config.db_connection);
  from_db(parents, config.db_connection);
  befreiungen.from_db();

  auto parent = std::find_if(parents.begin(), parents.end(), 
               [&password](std::pair<const int, eltern>& e){return e.second.password() == password;});

  if(parent == parents.end())
  {
    Wt::WMessageBox *messageBox = new Wt::WMessageBox
      ("User not found", "", Wt::Information, Wt::Yes );
    messageBox->setModal(false);
    messageBox->buttonClicked().connect(std::bind([=] () { delete messageBox; }));
    messageBox->show(); //TODO: This does not work
    throw std::runtime_error("User not found");
  }
  
  //Get browser locale
  auto browser_locale = app->locale().name();

  app->setTitle(" Eltern Urlaub");
  HolidaysWidget * holidays = new HolidaysWidget(parent->second, befreiungen, browser_locale, app->root());

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

