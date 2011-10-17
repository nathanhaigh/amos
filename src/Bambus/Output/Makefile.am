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
	-I$(top_srcdir)/src/CelMsg \
	-I$(top_srcdir)/src/Slice \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_srcdir)/src/GNU \
	-I$(top_srcdir)/src/Bambus/Bundler \
        -I$(top_srcdir)/src/Align \
	$(BOOST_CXXFLAGS)  \
	-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	-pedantic

##-- OutputMotifs \
OutputMotifs_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
OutputMotifs_SOURCES = \
	Position.cc Position.hh Align_Utils.cc Align_Utils.hh Motif_Utils.cc Motif_Utils.hh OutputMotifs.cc

##-- OutputScaffolds \
OutputScaffolds_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
OutputScaffolds_SOURCES = \
	Position.cc Position.hh Align_Utils.cc Align_Utils.hh Motif_Utils.cc Motif_Utils.hh OutputScaffolds.cc

OutputResults_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Common/libCommon.a \
        $(top_builddir)/src/AMOS/libAMOS.a \
        $(top_builddir)/src/GNU/libGNU.a
OutputResults_SOURCES = \
        Output_Utils.cc Output_Utils.hh OutputResults.cc

Linearize_LDADD = \
	$(top_builddir)/src/Bambus/Bundler/libBundler.a \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
Linearize_SOURCES = \
	Position.cc Position.hh Align_Utils.cc Align_Utils.hh Motif_Utils.cc Motif_Utils.hh Linearize.cc

##-- END OF MAKEFILE --##
