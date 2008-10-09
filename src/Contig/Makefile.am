##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

amoslib_LIBRARIES = \
	libContigUtils.a \
    libDataStore.a


amosinclude_HEADERS = \
	ContigUtils.hh \
    DataStore.hh \
    CoverageStats.hh \
    Insert.hh  \
    InsertStats.hh




##-- TO BE INSTALLED
dist_bin_SCRIPTS = \
	zipalign.pl \
    autoJoiner.pl


##-- TO BE INSTALLED
bin_PROGRAMS = \
    olapsFromContig \
    fattenContig \
    insertGapColumn \
    manageContigs \
    revContig \
    rotateContig \
    splitContig \
    stitchContigs \
    trimContig \
    zipContigs 


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Slice \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
	-I$(top_srcdir)/src/Foundation

libContigUtils_a_SOURCES = \
    ContigUtils.cc

libDataStore_a_SOURCES = \
     DataStore.cc \
     CoverageStats.cc \
     Insert.cc        \
     InsertStats.cc 

##-- olapsFromContig
olapsFromContig_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
olapsFromContig_SOURCES = \
	olapsFromContig.cc

##-- fattenContig
fattenContig_LDADD = \
    $(top_builddir)/src/Contig/libContigUtils.a \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a
fattenContig_SOURCES = \
    ContigSequence.hh \
    ContigSequence.cc \
    tigrinc.hh \
    tigrinc.cc \
    ContigFattener.hh \
    ContigFattener.cc \
    fattenContig.cc  \
    sw_align.hh \
    sw_alignscore.hh \
    sw_align.cc


##-- insertGapColumn
insertGapColumn_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
insertGapColumn_SOURCES = \
	insertGapColumn.cc 

##-- manageContigs
manageContigs_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a
manageContigs_SOURCES = \
	manageContigs.cc 


##-- revContig
revContig_LDADD = \
    $(top_builddir)/src/Contig/libContigUtils.a \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
revContig_SOURCES = \
	revContig.cc 


##-- rotateContig
rotateContig_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
rotateContig_SOURCES = \
    rotateContig.cc


##-- splitContig
splitContig_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
splitContig_SOURCES = \
	splitContig.cc 

##-- trimContig
stitchContigs_LDADD = \
	$(top_builddir)/src/Contig/libContigUtils.a \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a
stitchContigs_SOURCES = \
	stitchContigs.cc 



##-- trimContig
trimContig_LDADD = \
	$(top_builddir)/src/Contig/libContigUtils.a \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a
trimContig_SOURCES = \
	trimContig.cc 


##-- zipcontigs
zipContigs_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a
zipContigs_SOURCES = \
	zipContigs.cc 
