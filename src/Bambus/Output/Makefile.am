##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
	FastaGenerator

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
	-I$(top_builddir)/src/Bambus/Bundler \
   	$(BOOST_CXXFLAGS)  \
	-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	-pedantic

##-- FastaGenerator\
FastaGenerator_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
FastaGenerator_SOURCES = \
	Position.cc Position.hh FastaGenerator.cc

##-- END OF MAKEFILE --##
