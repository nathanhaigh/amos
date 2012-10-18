##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

amoslib_LIBRARIES = \
	libContigUtils.a \
    libDataStore.a


amosinclude_HEADERS = \
	ContigUtils.hh \
    DataStore.hh \
    CoverageStats.hh \
    Insert.hh  \
    InsertStats.hh


##-- TO BE INSTALLED
bin_PROGRAMS = \
    olapsFromContig 


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Slice \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
	-I$(top_srcdir)/src/Foundation

libContigUtils_a_SOURCES = \
    ContigUtils.cc

libDataStore_a_SOURCES = \
     DataStore.cc \
     CoverageStats.cc \
     Insert.cc        \
     InsertStats.cc 

##-- olapsFromContig
olapsFromContig_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
olapsFromContig_SOURCES = \
	olapsFromContig.cc

