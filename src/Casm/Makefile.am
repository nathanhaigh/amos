##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	layout-align \
	layout-paths

noinst_PROGRAMS = \
	libcalc


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS


#-- layout-align
layout_align_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
layout_align_SOURCES = \
	layout-align.cc


#-- layout-paths
layout_paths_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
layout_paths_SOURCES = \
	layout-paths.cc


#-- libcalc
libcalc_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
libcalc_SOURCES = \
	libcalc.cc

##-- END OF MAKEFILE --##