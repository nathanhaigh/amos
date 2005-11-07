##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
dist_bin_SCRIPTS = \
	rerunMultiTest.pl \
	runMultiTest.pl \
	runTest.pl \
	summarizeMultiTest.pl

bin_PROGRAMS = \
	pyrosim


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
        -I$(top_srcdir)/src/Common \
        -I$(top_srcdir)/src/AMOS

pyrosim_LDADD = \
        $(top_builddir)/src/Common/libCommon.a \
        $(top_builddir)/src/AMOS/libAMOS.a
pyrosim_SOURCES = \
        pyrosim.cc


##-- END OF MAKEFILE --##