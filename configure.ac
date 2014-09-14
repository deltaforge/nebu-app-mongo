# Define basic library information
AC_PREREQ(2.61)
AC_INIT([mongo-nebu], [0.1], [])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 foreign subdir-objects])

# Check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S

# Check for libraries
PKG_CHECK_MODULES([TINYXML2], [tinyxml2])
AC_SUBST([TINYXML2_CFLAGS])
AC_SUBST([TINYXML2_LIBS])
PKG_CHECK_MODULES([NEBU_APP_FRAMEWORK], [libnebu-app-framework])
AC_SUBST([NEBU_APP_FRAMEWORK_CFLAGS])
AC_SUBST([NEBU_APP_FRAMEWORK_LIBS])
PKG_CHECK_MODULES([LOG4CXX], [liblog4cxx])
AC_SUBST([LOG4CXX_CFLAGS])
AC_SUBST([LOG4CXX_LIBS])

# Define test configuration option
AC_ARG_ENABLE([tests],
	[	--enable-tests   Turn on test and coverage flags],
	[case "${enableval}" in
	  yes) tests=true ;;
	  no)  tests=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-tests]) ;;
	esac],[tests=false])
AM_CONDITIONAL([TESTS_ENABLED], [test x$tests = xtrue])

# Define mocking option
AC_ARG_ENABLE([mocks],
	[	--enable-mocks   Turn on mocking of SSH and Mongo calls],
	[case "${enableval}" in
	  yes) mocks=true ;;
	  no)  mocks=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-mocks]) ;;
	esac],[mocks=false])
AM_CONDITIONAL([MOCKS_ENABLED], [test x$mocks = xtrue])
AM_COND_IF([MOCKS_ENABLED], AC_DEFINE([MOCKING], [], [Defines MOCKING in the config header]))

# Define Makefiles
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	test/Makefile
])

# Generate 'configure' 
AC_OUTPUT
