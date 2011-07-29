##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	trimByOvl \
	runAmos \
	analyzeSNPs \
    recallConsensus \
	cvgStat \
    tagsum-reduce


dist_bin_SCRIPTS = \
	benchmark_qual.pl \
	benchmark_seq.pl \
	ctgovl.pl \
    filter_contig.pl \
    filterfrg.pl \
	filter_seq.pl \
    listcontigreads.sh \
	cvgChop.pl \
	getN50.pl \
	getlengths.pl \
    gccontent.pl \
	preTA.pl \
	pullTArchive.pl \
	runTA.pl \
	preassembleFrgs.pl \
	ktrimfrg.pl \
    translate-fasta.pl \
	delta2cvg.pl \
	delta2clr.pl \
	updateDeltaClr.pl \
	nucmerAnnotate.pl \
	nucmer2ovl.pl \
	sort2.pl \
	ovl2OVL.pl \
    fasta_select_len.pl \
    fasta_to_fastq.pl \
    fastqqc.pl \
    fastq_to_fasta_fast.pl \
    fixfastq.pl \
    fastq_rename.pl \
    stats.pl \
    revFasta.pl

# runAmos.pl 

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/AMOS

##-- analyzeSNPs
analyzeSNPs_CPPFLAGS = \
	-I$(top_srcdir)/src/GNU \
	-I$(top_builddir)/src/GNU \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_srcdir)/src/Common \
    -I$(top_srcdir)/src/Slice

analyzeSNPs_LDADD = \
        $(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Common/libCommon.a \
        $(top_builddir)/src/Slice/libSlice.a

analyzeSNPs_SOURCES = \
	analyzeSNPs.cc

##-- recallConsensus
recallConsensus_CPPFLAGS = \
	-I$(top_builddir)/src/GNU \
	-I$(top_srcdir)/src/GNU \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_srcdir)/src/Common \
    -I$(top_srcdir)/src/Slice

recallConsensus_LDADD = \
        $(top_builddir)/src/AMOS/libAMOS.a \
	    $(top_builddir)/src/GNU/libGNU.a \
	    $(top_builddir)/src/Common/libCommon.a \
        $(top_builddir)/src/Slice/libSlice.a

recallConsensus_SOURCES = \
	recallConsensus.cc


##-- trimByOvl
trimByOvl_CPPFLAGS = \
	-I$(top_srcdir)/src/AMOS
trimByOvl_LDADD = \
        $(top_builddir)/src/AMOS/libAMOS.a
trimByOvl_SOURCES = \
	trimByOvl.cc

##-- cvgStat
cvgStat_CPPFLAGS = \
	-I$(top_builddir)/src/GNU \
	-I$(top_srcdir)/src/AMOS
cvgStat_LDADD = \
        $(top_builddir)/src/AMOS/libAMOS.a
cvgStat_SOURCES = \
	cvgStat.cc

##-- tagsum-reduce
tagsum_reduce_SOURCES = \
	tagsum-reduce.cc

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
