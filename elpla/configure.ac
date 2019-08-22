
AC_INIT([Mafalda Elterndienstplan], [1.0.0], [mafalda@engarda.org], [mafalda_ed])

AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])

# Checks for programs.
LT_INIT
AC_SUBST(LIBTOOL_DEPS)
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11(noext)
AC_LANG([C++])

AC_CHECK_HEADERS([Wt/WApplication], [], [AC_MSG_ERROR([Couldn't find wt include files])])
AC_CHECK_HEADERS([cppdb/frontend.h], [], [AC_MSG_ERROR([Couldn't find cppdb include files])])
PKG_CHECK_MODULES(vmime, [vmime = 0.9.1])


AC_CONFIG_FILES(Makefile
                src/Makefile)

AC_OUTPUT
