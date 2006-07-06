##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
    barcode-graph \
    build-persistent-bank \
    persistent-read-dist \
    persistent-fix-contigs \
    read-evolution \
    gap-closure-reads \
    mate-evolution

dist_bin_SCRIPTS = \
    persistent-assembly.pl \
    persistent-fix.pl

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
    -I$(top_srcdir)/src/Contig \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
    -I$(top_srcdir)/src/Foundation \
    -I$(top_srcdir)/src/bankViewer

##-- barcode-graph
barcode_graph_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

barcode_graph_SOURCES = \
	barcode-graph.cc

##-- build-persistent-bank
build_persistent_bank_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

build_persistent_bank_SOURCES = \
	build-persistent-bank.cc

##-- persistent-fix-contigs
persistent_fix_contigs_LDADD = \
    $(top_builddir)/src/Contig/libContigUtils.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

persistent_fix_contigs_SOURCES = \
	persistent-fix-contigs.cc \
    PersistentUtils.hh

##-- persistent_read_dist
persistent_read_dist_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

persistent_read_dist_SOURCES = \
	persistent-read-dist.cc \
    PersistentUtils.hh

##-- read-evolution
read_evolution_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

read_evolution_SOURCES = \
	read-evolution.cc \
    PersistentUtils.hh

##-- gap-closure-reads
gap_closure_reads_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

gap_closure_reads_SOURCES = \
	gap-closure-reads.cc


##-- mate-evolution
mate_evolution_LDADD = \
	$(top_builddir)/src/GNU/libGNU.a \
    $(top_builddir)/src/bankViewer/libDataStore.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
    $(top_builddir)/src/Staden/read/libread.a


mate_evolution_SOURCES = \
	mate-evolution.cc \
    PersistentUtils.hh

##-- END OF MAKEFILE --##
