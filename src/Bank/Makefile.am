##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	bank2contig  \
	bank2lib \
	bank2coverage \
	bank2fasta \
	bank2scaff \
	bank-clean \
	bank-combine \
	bank-mapping \
	bank-report \
	bank-transact \
	bank-tutorial \
	dumpFeatures \
	dumpmates \
	dumpreads \
	extractContig \
	findMissingMates \
	grow-readbank \
	listReadPlacedStatus \
	listGCContent \
	load-overlaps \
	loadFeatures \
	normalizeScaffold \
	renameReads \
	resetFragLibrary \
	revScaffold \
	select-reads \
	simplifyLibraries \
	simpleContigLoader \
	updateBankPositions \
	updateClrRanges \
	updateLibSizes \
	dumpContigsAsReads \
	partitionBank


dist_bin_SCRIPTS = \
	bank-unlock.pl


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/CelMsg \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/Slice \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
	-I$(top_srcdir)/src/Foundation



##-- bank-combine
bank_combine_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_combine_SOURCES = \
	bank-combine.cc

##-- bank-mapping
bank_mapping_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_mapping_SOURCES = \
	bank-mapping.cc

##-- bank2contig
bank2contig_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
    $(top_builddir)/src/Foundation/libAMOSFoundation.a
bank2contig_SOURCES = \
	bank2contig.cc

##-- bank2lib
bank2lib_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a
bank2lib_SOURCES = \
	bank2lib.cc

##-- bank2coverage
bank2coverage_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/Slice/libSlice.a
bank2coverage_SOURCES = \
	bank2coverage.cc

##-- bank2scaff
bank2scaff_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank2scaff_SOURCES = \
	bank2scaff.cc

##-- bank-clean
bank_clean_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_clean_SOURCES = \
	bank-clean.cc

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

##-- normalizeScaffold
normalizeScaffold_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
normalizeScaffold_SOURCES = \
	normalizeScaffold.cc

##-- revScaffold
revScaffold_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
revScaffold_SOURCES = \
	revScaffold.cc

##-- updateBankPositions
updateBankPositions_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
updateBankPositions_SOURCES = \
	updateBankPositions.cc

##-- simpleContigLoader
simpleContigLoader_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
simpleContigLoader_SOURCES = \
	simpleContigLoader.cc

##-- updateClrRanges
updateClrRanges_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
updateClrRanges_SOURCES = \
	updateClrRanges.cc

##-- updateLibSizes
updateLibSizes_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
updateLibSizes_SOURCES = \
	updateLibSizes.cc

##-- resetFragLibrary
resetFragLibrary_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
resetFragLibrary_SOURCES = \
	resetFragLibrary.cc

##-- extractContig
extractContig_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
extractContig_SOURCES = \
	extractContig.cc

##-- findMissingMates
findMissingMates_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
findMissingMates_SOURCES = \
	findMissingMates.cc

##-- listGCContent
listGCContent_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
listGCContent_SOURCES = \
	listGCContent.cc

    
##-- listReadPlacedStatus
listReadPlacedStatus_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
listReadPlacedStatus_SOURCES = \
	listReadPlacedStatus.cc

##-- dumpmates
dumpmates_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
dumpmates_SOURCES = \
	dumpmates.cc

##-- dumpFeatures
dumpFeatures_LDADD = \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
dumpFeatures_SOURCES = \
	dumpFeatures.cc

##-- loadFeatures
loadFeatures_LDADD = \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
loadFeatures_SOURCES = \
	loadFeatures.cc

##-- bank-tutorial
bank_tutorial_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_tutorial_SOURCES = \
	bank-tutorial.cc

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

##-- renameReads
renameReads_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
renameReads_SOURCES = \
	renameReads.cc

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
bank2fasta_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU
bank2fasta_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
bank2fasta_SOURCES = \
	bank2fasta.cc

##-- dumpContigsAsReads
dumpContigsAsReads_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU
dumpContigsAsReads_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
dumpContigsAsReads_SOURCES = \
	dumpContigsAsReads.cc

##- partitionBank 
partitionBank_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU
partitionBank_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
partitionBank_SOURCES = \
	partitionBank.cc

##-- simplifyLibraries
simplifyLibraries_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU
simplifyLibraries_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
simplifyLibraries_SOURCES = \
	simplifyLibraries.cc

##-- END OF MAKEFILE --##
