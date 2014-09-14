
AM_CPPFLAGS = -pthread -I\$(top_srcdir)/include -std=c++0x -Wall -Wextra \
	$(NEBU_APP_FRAMEWORK_CFLAGS) $(LOG4CXX_CFLAGS) $(TINYXML2_CFLAGS)
AM_LDFLAGS = -pthread 
if TESTS_ENABLED
AM_CPPFLAGS += --coverage -O0 -g -fno-default-inline -fno-inline
AM_LDFLAGS += --coverage
else
AM_CPPFLAGS += -O2
endif

src_SOURCES = \
	daemonDeployer.cpp \
	daemonManager.cpp \
	mongoAdapter.cpp \
	mongoApplication.cpp \
	mongocfgDaemon.cpp \
	mongoDaemon.cpp \
	mongodDaemon.cpp \
	mongosDaemon.cpp \
	replicaSet.cpp \
	replicaSetManager.cpp

bin_PROGRAMS = $(top_builddir)/nebu-mongo

__top_builddir__nebu_mongo_SOURCES = $(src_SOURCES)
__top_builddir__nebu_mongo_LDADD = -lmongoclient -lboost_system -lboost_thread -lboost_filesystem
__top_builddir__nebu_mongo_LDADD += $(NEBU_APP_FRAMEWORK_LIBS) $(LOG4CXX_LIBS) $(TINYXML2_LIBS) -lrestclient-cpp
