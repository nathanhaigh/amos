##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = tigger

##-- TO BE TESTED
check_SCRIPTS =


##-- GLOBAL INCLUDE
AM_CPPFLAGS = -I$(top_srcdir)/src/AMOS -I$(top_srcdir)/src/Graph


##-- tigger
tigger_LDADD = \
	$(top_builddir)/src/Graph/libGraph.a \
	$(top_builddir)/src/AMOS/libAMOS.a
tigger_SOURCES = Unitigger.cc Unitigger.hh Overlap.hh Read.hh


##-- END OF MAKEFILE --##