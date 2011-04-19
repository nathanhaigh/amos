##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
	FastaGenerator \
	Linearize \
	TextOutput

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_srcdir)/src/GNU \
	-I$(top_srcdir)/src/Bambus/Bundler \
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

TextOutput_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
        $(top_builddir)/src/AMOS/libAMOS.a \
        $(top_builddir)/src/GNU/libGNU.a
TextOutput_SOURCES = \
        Output_Utils.cc Output_Utils.hh TextOutput.cc

Linearize_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
Linearize_SOURCES = \
	Linearize.cc

##-- END OF MAKEFILE --##
