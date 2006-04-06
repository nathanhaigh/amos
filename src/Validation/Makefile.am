##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
	asmQC2 \
	asmQC \
    analyze-read-depth \
    find-duplicate-reads
#	shotgunSim

dist_bin_SCRIPTS = \
	coords2cam.pl \
	fixlib.pl \
	postCAqc.pl \
	tiling2cam.pl \
    cavalidate.acf \
    amosvalidate.acf \
    findTcovSnp.pl \
    clusterSnps.pl \
    listSurrogates.pl \
    suspiciousfeat2region.pl \
    find-query-breaks.pl
    

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
    -I$(top_srcdir)/src/Foundation

##-- analyze-read-depth
analyze_read_depth_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

analyze_read_depth_SOURCES = \
	analyze-read-depth.cc

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


##-- find-duplicate-reads
find_duplicate_reads_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU
find_duplicate_reads_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
find_duplicate_reads_SOURCES = \
	find-duplicate-reads.cc


##-- shotgunSim
#shotgunSim_LDADD = \
#	-lgsl \
#	-lgslcblas
#
#shotgunSim_SOURCES = \
#	shotgunSim.cc

##-- END OF MAKEFILE --##
