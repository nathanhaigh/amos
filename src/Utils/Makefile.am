##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	trimByOvl \
	runAmos \
	ContigIterator

dist_bin_SCRIPTS = \
	benchmark_qual.pl \
	benchmark_seq.pl \
	ctgovl.pl \
	cvgChop.pl \
	getN50.pl \
	getlengths.pl \
	preTA.pl \
	pullTArchive.pl \
	runTA.pl \
	preassembleFrgs.pl \
	ktrimfrg.pl
# runAmos.pl 


##-- GLOBAL INCLUDE
AM_CPPFLAGS =


##-- trimByOvl
trimByOvl_CPPFLAGS = \
	-I$(top_srcdir)/src/AMOS
trimByOvl_LDADD = \
        $(top_builddir)/src/AMOS/libAMOS.a
trimByOvl_SOURCES = \
	trimByOvl.cc

##-- ContigIterator
ContigIterator_CPPFLAGS = \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU
ContigIterator_LDADD = \
        $(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
ContigIterator_SOURCES = \
	ContigIterator.cc

##-- runAmos
## note: this references top_builddir because getopt.h is built if needed
runAmos_CPPFLAGS = \
	-I$(top_builddir)/src/GNU
        ## because some system headers will be built if necessary
runAmos_LDADD = \
	$(top_builddir)/src/GNU/libGNU.a
runAmos_SOURCES = \
	runAmos.cc


##-- END OF MAKEFILE --##
