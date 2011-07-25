##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	arrive \
	arrive2 \
    genome-complexity \
    genome-complexity-fast \
    count-kmers \
    find-tandem \
    count-qmers \
    qmer-filter \
	frg-umd-merge \
	gap-links \
	hash-overlap \
	kmer-count \
	kmer-cov \
	kmer-cov-plot \
	make-consensus \
	make-consensus_poly \
	merge-contigs \
	missing-reads \
	ovl-degr-dist \
    overlap-align \
	readinfo2cam \
	running-cmp \
	show-ma-asm \
	sim-cover-depth \
	sim-cover2 \
	sim-shotgun \
	simple-overlap \
    verify-layout

amoslib_LIBRARIES = \
	libAlign.a \
	libAlign_poly.a

amosinclude_HEADERS = \
	align.hh \
	align_poly.hh \
	matrix.hh


##-- TO BE TESTED
check_PROGRAMS = \
	maligntest \
	test-align


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/CelMsg \
	-I$(top_srcdir)/src/Slice \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
	-I$(top_srcdir)/src/Foundation



##-- arrive
arrive_SOURCES = \
	arrive.cc

##-- arrive2
arrive2_LDADD = \
	$(top_builddir)/src/Common/libCommon.a
arrive2_SOURCES = \
	arrive2.cc

##-- count-kmers
count_kmers_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
    $(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
count_kmers_SOURCES = \
	count-kmers.cc

##-- find-tandem
find_tandem_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
    $(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
find_tandem_SOURCES = \
	find-tandem.cc

##-- count-qmers
count_qmers_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
    $(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
count_qmers_SOURCES = \
	count-qmers.cc

##-- qmer-filter
qmer_filter_SOURCES = \
	qmer-filter.cc

##-- frg-umd-merge
frg_umd_merge_LDADD = \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
frg_umd_merge_SOURCES = \
	frg-umd-merge.cc

##-- gap-links
gap_links_LDADD = \
	$(top_builddir)/src/Common/libCommon.a
gap_links_SOURCES = \
	gap-links.cc

##-- genome-complexity
genome_complexity_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
    	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a
genome_complexity_SOURCES = \
	genome-complexity-fast.cc

##-- genome-complexity-fast
genome_complexity_fast_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
    	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a
genome_complexity_fast_SOURCES = \
	genome-complexity-fast.cc

##-- hash-overlap
hash_overlap_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
hash_overlap_SOURCES = \
	hash-overlap.cc \
	hash-overlap.hh

##-- kmer-count
kmer_count_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
kmer_count_SOURCES = \
	kmer-count.cc

##-- kmer-cov
kmer_cov_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
kmer_cov_SOURCES = \
	kmer-cov.cc

##-- kmer-cov-plot
kmer_cov_plot_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
kmer_cov_plot_SOURCES = \
	kmer-cov-plot.cc

##-- overlap-align
overlap_align_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
overlap_align_SOURCES = \
	overlap-align.cc

##-- make-consensus
make_consensus_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
make_consensus_SOURCES = \
	make-consensus.cc

##-- make-consensus_poly
make_consensus_poly_LDADD = \
	$(top_builddir)/src/Align/libAlign_poly.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
make_consensus_poly_SOURCES = \
	make-consensus_poly.cc

##-- maligntest
maligntest_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
maligntest_SOURCES = \
	maligntest.cc

##-- merge-contigs
merge_contigs_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
merge_contigs_SOURCES = \
	merge-contigs.cc

##-- missing-reads
missing_reads_SOURCES = \
	missing-reads.cc

##-- ovl-degr-dist
ovl_degr_dist_LDADD = \
	$(top_builddir)/src/Common/libCommon.a
ovl_degr_dist_SOURCES = \
	ovl-degr-dist.cc

##-- readinfo2cam
readinfo2cam_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
readinfo2cam_SOURCES = \
	readinfo2cam.cc

##-- running-cmp
running_cmp_LDADD = \
	$(top_builddir)/src/Common/libCommon.a
running_cmp_SOURCES = \
	running-cmp.cc

##-- show-ma-asm
show_ma_asm_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
show_ma_asm_SOURCES = \
	show-ma-asm.cc

##-- sim-cover-depth
sim_cover_depth_LDADD = \
	$(top_builddir)/src/Common/libCommon.a
sim_cover_depth_SOURCES = \
	sim-cover-depth.cc

##-- sim-cover2
sim_cover2_LDADD = \
	$(top_builddir)/src/Common/libCommon.a
sim_cover2_SOURCES = \
	sim-cover2.cc

##-- sim-shotgun
sim_shotgun_LDADD = \
	$(top_builddir)/src/Common/libCommon.a
sim_shotgun_SOURCES = \
	sim-shotgun.cc

##-- simple-overlap
simple_overlap_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
simple_overlap_SOURCES = \
	simple-overlap.cc \
	simple-overlap.hh

##-- test-align
test_align_LDADD = \
	$(top_builddir)/src/Align/libAlign.a \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Slice/libSlice.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
test_align_SOURCES = \
	test-align.cc

##-- verify-layout
verify_layout_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a
verify_layout_SOURCES = \
	verify-layout.cc

##-- libAlign.a
libAlign_a_SOURCES = \
	align.cc

##-- libAlign_poly.a
libAlign_poly_a_SOURCES = \
	align_poly.cc

##-- END OF MAKEFILE --##
