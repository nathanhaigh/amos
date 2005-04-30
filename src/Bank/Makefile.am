##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	bank-clean \
	bank-index \
	bank-mapping \
	bank-report \
	bank-transact \
	dumpmates \
	select-reads \
	dumpreads \
	grow-readbank \
	load-overlaps \
	bank2contig  \
	bank2coverage \
	updateBankPositions \
	extractContig \
	bank2fasta \
    findMissingMates \
    updateClrRanges

#       dumpFeatures
#	loadFeatures 

dist_bin_SCRIPTS = \
	bank-unlock.pl


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/CelMsg \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/Slice \
	-I$(top_srcdir)/src/AMOS


##-- bank-mapping
bank_mapping_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_mapping_SOURCES = \
	bank-mapping.cc

##-- bank2contig
bank2contig_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank2contig_SOURCES = \
	bank2contig.cc

##-- bank2coverage
bank2coverage_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Slice/libSlice.a
bank2coverage_SOURCES = \
	bank2coverage.cc

##-- bank-clean
bank_clean_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_clean_SOURCES = \
	bank-clean.cc

##-- bank-index
bank_index_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_index_SOURCES = \
	bank-index.cc

##-- bank-report
bank_report_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_report_SOURCES = \
	bank-report.cc

##-- bank-transact
bank_transact_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_transact_SOURCES = \
	bank-transact.cc

##-- updateBankPositions
updateBankPositions_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
updateBankPositions_SOURCES = \
	updateBankPositions.cc

##-- updateClrRanges
updateClrRanges_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
updateClrRanges_SOURCES = \
	updateClrRanges.cc

##-- extractContig
extractContig_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
extractContig_SOURCES = \
	extractContig.cc

##-- loadFeatures
#loadFeatures_LDADD = \
#	$(top_builddir)/src/Common/libCommon.a \
#	$(top_builddir)/src/AMOS/libAMOS.a
#loadFeatures_SOURCES = \
#	loadFeatures.cc

##-- dumpFeatures
#dumpFeatures_LDADD = \
#	$(top_builddir)/src/Common/libCommon.a \
#	$(top_builddir)/src/AMOS/libAMOS.a
#dumpFeatures_SOURCES = \
#	dumpFeatures.cc

##-- findMissingMates
findMissingMates_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
findMissingMates_SOURCES = \
	findMissingMates.cc

##-- dumpmates
dumpmates_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
dumpmates_SOURCES = \
	dumpmates.cc

##-- select-reads
select_reads_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
select_reads_SOURCES = \
	select-reads.cc

##-- dumpreads
dumpreads_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
dumpreads_SOURCES = \
	dumpreads.cc

##-- grow-readbank
grow_readbank_LDADD = \
	$(top_builddir)/src/CelMsg/libCelMsg.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
grow_readbank_SOURCES = \
	grow-readbank.cc

##-- load-overlaps
load_overlaps_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
load_overlaps_SOURCES = \
	load-overlaps.cc

##-- bank2fasta
bank2fasta_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank2fasta_SOURCES = \
	bank2fasta.cc

##-- END OF MAKEFILE --##
