##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	contig-cmp


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common


##-- contig-cmp
contig_cmp_LDADD = \
	$(top_builddir)/src/Common/libCommon.a
contig_cmp_SOURCES = \
	contig-cmp.cc


##-- END OF MAKEFILE --##