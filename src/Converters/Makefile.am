##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

bin_PROGRAMS = \
	toAmos_experimental

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common

##-- TO BE INSTALLED
dist_bin_SCRIPTS = \
	ace2contig.pl \
	agp2amos.pl \
	amos2frg.pl \
	amos2mates.pl \
	amos2sq.pl \
        amos2ace.pl \
	arachne2ctg.pl \
	arachne2scaff.pl \
	benchmark2arachne.pl \
	benchmark2ca.pl \
	benchmark2mates.pl \
	benchmark2ta.pl \
	ca2ctg.pl \
	ca2mates.pl \
	ca2scaff.pl \
	ca2ta.pl \
	cgb2ctg.pl \
	contig2contig.pl \
	ctg2fasta.pl \
	ctg2umdcontig.pl \
	frg2fasta.pl \
	frg2ta.pl \
	ta2ace.pl \
	tarchive2amos.pl \
	tarchive2ca.pl \
	toAmos.pl \
	toArachne.pl \
	scaff2fasta.pl \
	parsecasm.pl \
	castats.pl \
	ca2singletons.pl \
	phd2afg.pl

##-- GLOBAL INCLUDE

##-- toAmos
toAmos_experimental_CPPFLAGS = \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_srcdir)/src/Common
toAmos_experimental_LDADD = \
        $(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	-lexpat
toAmos_experimental_SOURCES = \
        toAmos_experimental.cc

##-- END OF MAKEFILE --##
