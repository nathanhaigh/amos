##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
amoslib_LIBRARIES = \
	libAMOS.a

amosinclude_HEADERS = \
	BankStream_AMOS.hh \
	Bank_AMOS.hh \
	ContigEdge_AMOS.hh \
    ContigIterator_AMOS.hh \
	ContigLink_AMOS.hh \
	Motif_AMOS.hh \
	Contig_AMOS.hh \
	Edge_AMOS.hh \
	Feature_AMOS.hh \
	Fragment_AMOS.hh \
	Group_AMOS.hh \
	IDMap_AMOS.hh \
	Index_AMOS.hh \
	Kmer_AMOS.hh \
	Layout_AMOS.hh \
	Library_AMOS.hh \
	Link_AMOS.hh \
	Message_AMOS.hh \
	Overlap_AMOS.hh \
	Read_AMOS.hh \
	ScaffoldEdge_AMOS.hh \
	ScaffoldLink_AMOS.hh \
	Scaffold_AMOS.hh \
	Sequence_AMOS.hh \
	Universal_AMOS.hh \
	databanks_AMOS.hh \
	datatypes_AMOS.hh \
	exceptions_AMOS.hh \
	foundation_AMOS.hh \
	inttypes_AMOS.hh \
	messages_AMOS.hh \
	universals_AMOS.hh \
	utility_AMOS.hh


##-- TO BE TESTED
check_PROGRAMS = \
	banktest \
	maptest \
	msgtest \
	streamtest \
	umdtest


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common


##-- banktest
banktest_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
banktest_SOURCES = \
	banktest.cc

##-- maptest
maptest_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
maptest_SOURCES = \
	maptest.cc

##-- msgtest
msgtest_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
msgtest_SOURCES = \
	msgtest.cc

##-- streamtest
streamtest_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
streamtest_SOURCES = \
	streamtest.cc

##-- umdtest
umdtest_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
umdtest_SOURCES = \
	umdtest.cc

##-- libAMOS.a
libAMOS_a_CPPFLAGS =
	-I$(top_builddir)/src/GNU
        ## because some system headers will be built if necessary
libAMOS_a_LIBADD = \
	$(LIBOBJS:%=$(top_builddir)/src/GNU/%)
libAMOS_a_SOURCES = \
	BankStream_AMOS.cc \
	Bank_AMOS.cc \
	ContigEdge_AMOS.cc \
    ContigIterator_AMOS.cc \
	ContigLink_AMOS.cc \
	Motif_AMOS.cc \
	Contig_AMOS.cc \
	Edge_AMOS.cc \
	Feature_AMOS.cc \
	Fragment_AMOS.cc \
	Group_AMOS.cc \
	IDMap_AMOS.cc \
	Index_AMOS.cc \
	Kmer_AMOS.cc \
	Layout_AMOS.cc \
	Library_AMOS.cc \
	Link_AMOS.cc \
	Message_AMOS.cc \
	Overlap_AMOS.cc \
	Read_AMOS.cc \
	ScaffoldEdge_AMOS.cc \
	ScaffoldLink_AMOS.cc \
	Scaffold_AMOS.cc \
	Sequence_AMOS.cc \
	Universal_AMOS.cc \
	datatypes_AMOS.cc \
	universals_AMOS.cc \
	utility_AMOS.cc


##-- END OF MAKEFILE --##
