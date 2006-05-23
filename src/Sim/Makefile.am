##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
dist_bin_SCRIPTS = \
	rerunMultiTest.pl \
	runMultiTest.pl \
	runTest.pl \
	summarizeMultiTest.pl

bin_PROGRAMS = \
	pyrosim \
	tandemCollapse


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
        -I$(top_srcdir)/src/Common \
        -I$(top_srcdir)/src/AMOS \
	-I$(top_srcdir)/src/GNU

##-- pyrosim
pyrosim_LDADD = \
        $(top_builddir)/src/Common/libCommon.a \
        $(top_builddir)/src/AMOS/libAMOS.a
pyrosim_SOURCES = \
        pyrosim.cc

##-- tandemCollapse
#tandemCollapse_CPPFLAGS = \
#	$(top_builddir)/src/Common \
#	$(top_builddir)/src/AMOS \
#	$(top_builddir)/src/GNU
tandemCollapse_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
tandemCollapse_SOURCES = \
	tandemCollapse.cc

###-- shotgunSim
#shotgunSim_LDADD = \
#	-lgsl \
#	-lgslcblas
#
#shotgunSim_SOURCES = \
#	shotgunSim.cc

##-- END OF MAKEFILE --##
