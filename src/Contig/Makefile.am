##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	zipcontigs \
    revcontig \
    insertgapcontig \
    trimcontig \
    splitContig

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Slice \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_srcdir)/src/Foundation

##-- insertgapcontig
insertgapcontig_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
insertgapcontig_SOURCES = \
	insertgapcontig.cc 


##-- revcontig
revcontig_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
revcontig_SOURCES = \
	revcontig.cc 

##-- splitContig
splitContig_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
splitContig_SOURCES = \
	splitContig.cc 



##-- trimcontig
trimcontig_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
trimcontig_SOURCES = \
	trimcontig.cc 


##-- zipcontigs
zipcontigs_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
zipcontigs_SOURCES = \
	zipcontigs.cc 
