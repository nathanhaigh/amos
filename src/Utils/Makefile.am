##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	trimByOvl \
	runAmos

dist_bin_SCRIPTS = \
	benchmark_qual.pl \
	benchmark_seq.pl \
	ctgovl.pl \
	cvgChop.pl \
	getN50.pl \
	getlengths.pl \
	preTA.pl \
	pullTArchive.pl \
	runTA.pl
# runAmos.pl 


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
        -I$(top_srcdir)/src/AMOS


##-- trimByOvl
trimByOvl_LDADD = \
        $(top_builddir)/src/AMOS/libAMOS.a
trimByOvl_SOURCES = \
	trimByOvl.cc \
	runAmos.cc


##-- END OF MAKEFILE --##
