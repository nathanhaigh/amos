##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
	OutputMotifs \
	OutputScaffolds \
	Linearize \
	OutputResults

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_srcdir)/src/GNU \
	-I$(top_srcdir)/src/Bambus/Bundler \
	$(BOOST_CXXFLAGS)  \
	-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	-pedantic

##-- OutputMotifs \
OutputMotifs_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
OutputMotifs_SOURCES = \
	Position.cc Position.hh OutputMotifs.cc

##-- OutputMotifs \
OutputScaffolds_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
OutputScaffolds_SOURCES = \
	Position.cc Position.hh OutputScaffolds.cc

OutputResults_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
        $(top_builddir)/src/AMOS/libAMOS.a \
        $(top_builddir)/src/GNU/libGNU.a
OutputResults_SOURCES = \
        Output_Utils.cc Output_Utils.hh OutputResults.cc

Linearize_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
Linearize_SOURCES = \
	Linearize.cc

##-- END OF MAKEFILE --##
