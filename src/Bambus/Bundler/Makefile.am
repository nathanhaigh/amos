##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
amoslib_LIBRARIES = \
	libBundler.a

amosinclude_HEADERS = \
	Utilities_Bundler.hh

bin_PROGRAMS = \
	Bundler \
	clk \
	MarkRepeats	\
	FilterEdgesByCluster \
	OrientContigs

dist_bin_SCRIPTS = \
	Chainer.pl \
	Joiner.pl


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_srcdir)/src/GNU \
	$(BOOST_CXXFLAGS)  \
	-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	-pedantic

##-- Bundler
Bundler_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
Bundler_SOURCES = \
	Bundler.cc


##-- CLK
clk_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
clk_SOURCES = \
	clk.cc

MarkRepeats_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
MarkRepeats_SOURCES = \
	MarkRepeats.cc

OrientContigs_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
OrientContigs_SOURCES = \
	OrientContigs.cc

FilterEdgesByCluster_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
FilterEdgesByCluster_SOURCES = \
	FilterEdgesByCluster.cc

##-- libBundler.a
libBundler_a_CPPFLAGS = \
	$(BOOST_CXXFLAGS)  \
	-I$(top_builddir)/src/GNU \
	-I$(top_builddir)/src/AMOS
	## because some system headers will be built if necessary
libBundler_a_LIBADD = \
	$(LIBOBJS:%=$(top_builddir)/src/GNU/%) \
	$(LIBOBJS:%=$(top_builddir)/src/AMOS/%)
libBundler_a_SOURCES = \
	Utilities_Bundler.cc

##-- END OF MAKEFILE --##
