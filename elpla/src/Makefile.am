
AUTOMAKE_OPTIONS = 1.8 foreign

LIBED = $(builddir)/libed.la

bin_PROGRAMS = ed_notify ed_webapp ed_holidays ed_presence ed_statistics

noinst_LTLIBRARIES = libed.la

ed_notify_SOURCES = ed_notify.cpp

ed_webapp_SOURCES = ed_webapp.cpp

ed_holidays_SOURCES = ed_holidays.cpp

ed_presence_SOURCES = ed_presence.cpp

ed_statistics_SOURCES = ed_statistics.cpp

libed_la_SOURCES = eltern.cpp parent_list.cpp ed_config.cpp notifier.cpp shift.cpp shift_list.cpp befreiung.cpp waiver_list.cpp closing_day.cpp closing_days.cpp ed_state.cpp

ed_notify_LDFLAGS = -pthread
ed_notify_LDADD = -lcppdb -lboost_program_options -lboost_date_time -lncurses -lvmime -lgsasl -lgnutls -lgsasl $(LIBED)
ed_notify_DEPENDENCIES = $(LIBED)

ed_webapp_LDFLAGS = -pthread
ed_webapp_LDADD = -lcppdb -lboost_program_options -lboost_date_time -lncurses -lvmime -lgsasl -lgnutls -lgsasl -lwt -lwthttp $(LIBED)
ed_webapp_DEPENDENCIES = $(LIBED)

ed_holidays_LDFLAGS = -pthread
ed_holidays_LDADD = -lcppdb -lboost_program_options -lboost_date_time -lncurses -lvmime -lgsasl -lgnutls -lgsasl -lwt -lwthttp $(LIBED)
ed_holidays_DEPENDENCIES = $(LIBED)

ed_presence_LDFLAGS = -pthread
ed_presence_LDADD = -lcppdb -lboost_program_options -lboost_date_time -lncurses -lvmime -lgsasl -lgnutls -lgsasl -lwt -lwthttp $(LIBED)
ed_presence_DEPENDENCIES = $(LIBED)

ed_statistics_LDFLAGS = -pthread
ed_statistics_LDADD = -lcppdb -lboost_program_options -lboost_date_time -lncurses -lvmime -lgsasl -lgnutls -lgsasl -lwt -lwthttp $(LIBED)
ed_statistics_DEPENDENCIES = $(LIBED)
