##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

amoslib_LIBRARIES = \
	libContigUtils.a

amosinclude_HEADERS = \
	ContigUtils.hh


##-- TO BE INSTALLED
dist_bin_SCRIPTS = \
	zipalign.pl


##-- TO BE INSTALLED
bin_PROGRAMS = \
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
	-I$(top_srcdir)/src/Foundation

libContigUtils_a_SOURCES = \
    ContigUtils.cc

##-- fattenContig
fattenContig_LDADD = \
    $(top_builddir)/src/Contig/libContigUtils.a \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
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
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
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
	$(top_builddir)/src/Foundation/libAMOSFoundation.a 
stitchContigs_SOURCES = \
	stitchContigs.cc 



##-- trimContig
trimContig_LDADD = \
    $(top_builddir)/src/Contig/libContigUtils.a \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
trimContig_SOURCES = \
	trimContig.cc 


##-- zipcontigs
zipContigs_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
zipContigs_SOURCES = \
	zipContigs.cc 
