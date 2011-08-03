##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
  po_align \
  fattenContig \
  insertGapColumn \
  manageContigs \
  revContig \
  rotateContig \
  splitContig \
  stitchContigs \
  trimContig \
  zipContigs \
  barcode-graph \
  build-persistent-bank \
  auto-fix-contigs \
  persistent-read-dist \
  persistent-fix-contigs \
  read-evolution \
  gap-closure-reads \
  mate-evolution \
  analyzeHaplotypes


dist_bin_SCRIPTS = \
  aj_evaluateSequenceOverlaps.pl \
  aj_joinContigs.pl \
  aj_lowcomplexityfilter.pl \
  aj_scaff2amos.pl \
  aj_scaff2fasta.pl \
  aligncestat.pl \
  aligndist.pl \
  alignextend.pl \
  autoJoiner.pl \
  blasr_alignquality.pl \
  coords_best.pl \
  coords_depth.pl \
  ctg_gc.pl \
  fasta_filter_raw.pl \
  fastq_to_mates.pl \
  filterfastadups.pl \
  link2reads.pl \
  persistent-assembly.pl \
  persistent-fix.pl \
  persistent-plot-breaks.pl \
  persistent-unitig.pl \
  splitscafffa.pl \
  zipalign.pl


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
	-I$(top_srcdir)/src/Foundation \
	-I$(top_srcdir)/src/Contig

##-- po-align
po_align_LDADD = \
    $(top_builddir)/src/Common/libCommon.a \
    $(top_builddir)/src/AMOS/libAMOS.a
po_align_SOURCES = \
    po-align.cc


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




##-- barcode-graph
barcode_graph_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a

barcode_graph_SOURCES = \
	barcode-graph.cc

##-- build-persistent-bank
build_persistent_bank_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a

build_persistent_bank_SOURCES = \
	build-persistent-bank.cc

##-- auto-fix-contigs
auto_fix_contigs_LDADD = \
	$(top_builddir)/src/Contig/libContigUtils.a \
	$(top_builddir)/src/Contig/libDataStore.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a

auto_fix_contigs_SOURCES = \
	auto-fix-contigs.cc \
    PersistentUtils.hh

##-- persistent-fix-contigs
persistent_fix_contigs_LDADD = \
	$(top_builddir)/src/Contig/libContigUtils.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a

persistent_fix_contigs_SOURCES = \
	persistent-fix-contigs.cc \
	PersistentUtils.hh

##-- persistent_read_dist
persistent_read_dist_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a

persistent_read_dist_SOURCES = \
	persistent-read-dist.cc \
    PersistentUtils.hh

##-- read-evolution
read_evolution_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a

read_evolution_SOURCES = \
	read-evolution.cc \
    PersistentUtils.hh

##-- gap-closure-reads
gap_closure_reads_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a

gap_closure_reads_SOURCES = \
	gap-closure-reads.cc


##-- mate-evolution
mate_evolution_LDADD = \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Contig/libDataStore.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/GNU/libGNU.a


mate_evolution_SOURCES = \
	mate-evolution.cc \
    PersistentUtils.hh

##-- analyzeHaplotypes 
analyzeHaplotypes_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
	-I$(top_srcdir)/src/Foundation \
	-I$(top_srcdir)/src/Contig \
    -I$(top_srcdir)/src/Slice 
                     
analyzeHaplotypes_LDADD = \
    $(top_builddir)/src/AMOS/libAMOS.a \
    $(top_builddir)/src/GNU/libGNU.a \
    $(top_builddir)/src/Common/libCommon.a \
    $(top_builddir)/src/Slice/libSlice.a 
                                              
analyzeHaplotypes_SOURCES = \
    analyzeHaplotypes.cc 

##-- END OF MAKEFILE --##
