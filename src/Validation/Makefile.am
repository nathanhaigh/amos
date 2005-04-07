es##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
	asmQC \
	shotgunSim

dist_bin_SCRIPTS = \
#	asmQC.pl \
	coords2cam.pl \
	fixlib.pl \
	postCAqc.pl \
	tiling2cam.pl

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


##-- shotgunSim
shotgunSim_LDADD = \
	-lgsl \
	-lgslcblas

shotgunSim_SOURCES = \
	shotgunSim.cc
##-- END OF MAKEFILE --##
