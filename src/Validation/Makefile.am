##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
	asmQC2 \
	asmQC \
    analyze-read-depth \
    find-duplicate-reads \
    cestat-cov \
    insert-sizes \
    listSingletonMates \
    library-histogram \
    list-linked-contigs


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
    -I$(top_srcdir)/src/Foundation \
    -I$(top_srcdir)/src/Contig

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


###-- shotgunSim
#shotgunSim_LDADD = \
#	-lgsl \
#	-lgslcblas
#
#shotgunSim_SOURCES = \
#	shotgunSim.cc


cestat_cov_SOURCES = \
  cestat-cov.cc

cestat_cov_LDADD = $(QT_LIBS) $(EXTRA_LDADD) \
                   $(top_builddir)/src/bankViewer/libDataStore.a \
                   $(top_builddir)/src/Common/libCommon.a   \
                   $(top_builddir)/src/AMOS/libAMOS.a

insert_sizes_SOURCES = \
  insert-sizes.cc

insert_sizes_LDADD = \
                     $(top_builddir)/src/bankViewer/libDataStore.a \
                     $(top_builddir)/src/Common/libCommon.a   \
                     $(top_builddir)/src/AMOS/libAMOS.a 

listSingletonMates_SOURCES = \
  listSingletonMates.cc

listSingletonMates_LDADD = \
                     $(top_builddir)/src/bankViewer/libDataStore.a \
                     $(top_builddir)/src/Common/libCommon.a   \
                     $(top_builddir)/src/AMOS/libAMOS.a 

library_histogram_SOURCES = \
  library-histogram.cc

library_histogram_LDADD =  \
                     $(top_builddir)/src/bankViewer/libDataStore.a \
                     $(top_builddir)/src/Common/libCommon.a   \
                     $(top_builddir)/src/AMOS/libAMOS.a 

list_linked_contigs_SOURCES = \
  list-linked-contigs.cc

list_linked_contigs_LDADD =  \
                     $(top_builddir)/src/bankViewer/libDataStore.a \
                     $(top_builddir)/src/Common/libCommon.a   \
                     $(top_builddir)/src/AMOS/libAMOS.a


