#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

// c++0x only, for std::bind
// #include <functional>

using namespace Wt;

/*
 * A simple hello world application class which demonstrates how to react
 * to events, read input, and give feed-back.
 */
class ElternDienstApplication : public WApplication
{
public:
  ElternDienstApplication(const WEnvironment& env);

private:
  void eltern_verwalten();
  void eltern_statistik();
  WLineEdit *nameEdit_;
  WText *greeting_;

  void greet();
};

/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
*/
ElternDienstApplication::ElternDienstApplication(const WEnvironment& env)
  : WApplication(env)
{
  setTitle("ElternDienst Elterndienstplannung. Admin Oberfläche"); // title

//  nameEdit_ = new WLineEdit(root());                     // allow text input
//  nameEdit_->setFocus();                                 // give focus

  WPushButton *button
    = new WPushButton("Eltern verwalten", root());      // create a button
  button->setMargin(5, Left);                            // add 5 pixels margin

  root()->addWidget(new WText("Hier kann man die ElternDienst Elterndienstplannung verwalten")); 
  root()->addWidget(new WBreak());                       // insert a line break

  WPushButton *button_stats
    = new WPushButton("Eltern Statistik", root());      // create a button
  button_stats->setMargin(5, Left);                            // add 5 pixels margin

  root()->addWidget(new WText("Hier kann man die ElternDienst Statistk schauen"));
  root()->addWidget(new WBreak());                       // insert a line break

  /*
   * Connect signals with slots
   *
   * - simple Wt-way
   */
  button->clicked().connect(this, &ElternDienstApplication::eltern_verwalten);
  button_stats->clicked().connect(this, &ElternDienstApplication::eltern_statistik);

}

void ElternDienstApplication::eltern_verwalten()
{
  /*
   * Update the text, using text input into the nameEdit_ field.
   */
//  greeting_->setText("Hello there, " + nameEdit_->text());
}

void ElternDienstApplication::eltern_statistik()
{
  /*
   * Update the text, using text input into the nameEdit_ field.
   */
//  greeting_->setText("Hello there, " + nameEdit_->text());
}

WApplication *createApplication(const WEnvironment& env)
{
  /*
   * You could read information from the environment to decide whether
   * the user has permission to start a new application
   */
  return new ElternDienstApplication(env);
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
  return WRun(argc, argv, &createApplication);
}

