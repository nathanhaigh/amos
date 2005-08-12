##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	trimByOvl \
	runAmos \
	analyzeSNPs

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

amoslib_LIBRARIES = \
    libUtils.a

amosinclude_HEADERS = \
    ContigIterator.hh

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/AMOS

libUtils_a_SOURCES = \
    ContigIterator.cc



##-- analyzeSNPs
analyzeSNPs_CPPFLAGS = \
	-I$(top_srcdir)/src/GNU \
	-I$(top_srcdir)/src/AMOS \
    -I$(top_srcdir)/src/Slice

analyzeSNPs_LDADD = \
        $(top_builddir)/src/Utils/libUtils.a \
        $(top_builddir)/src/AMOS/libAMOS.a \
	    $(top_builddir)/src/GNU/libGNU.a \
        $(top_builddir)/src/Slice/libSlice.a

analyzeSNPs_SOURCES = \
	analyzeSNPs.cc


##-- trimByOvl
trimByOvl_CPPFLAGS = \
	-I$(top_srcdir)/src/AMOS
trimByOvl_LDADD = \
        $(top_builddir)/src/AMOS/libAMOS.a
trimByOvl_SOURCES = \
	trimByOvl.cc


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
