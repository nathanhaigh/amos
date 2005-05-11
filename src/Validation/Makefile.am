##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
	shotgunSim \
	asmQC2 \
	asmQC

dist_bin_SCRIPTS = \
	coords2cam.pl \
	fixlib.pl \
	postCAqc.pl \
	tiling2cam.pl \
    cavalidate.acf \
    findTcovSnp.pl \
    clusterSnps.pl \
    listSurrogates.pl \
    suspiciousfeat2region.pl

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU

##-- asmQC
asmQC_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a

asmQC_SOURCES = \
	asmQC.cc

##-- asmQC2
asmQC2_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a

asmQC2_SOURCES = \
	asmQC2.cc

##-- shotgunSim
shotgunSim_LDADD = \
	-lgsl \
	-lgslcblas

shotgunSim_SOURCES = \
	shotgunSim.cc

##-- END OF MAKEFILE --##
