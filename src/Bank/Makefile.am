##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	bank-mapping \
	bank-report \
	bank-transact \
	dumpmates \
	dumpreads \
	grow-readbank \
	load-overlaps


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/CelMsg \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS


##-- bank-mapping
bank_mapping_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
bank_mapping_SOURCES = \
	bank-mapping.cc

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

##-- dumpmates
dumpmates_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
dumpmates_SOURCES = \
	dumpmates.cc

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


##-- END OF MAKEFILE --##