##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
    barcode-graph \
    build-persistant-bank

dist_bin_SCRIPTS = \
    persistant-assembly.pl

##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS \
	-I$(top_builddir)/src/GNU \
    -I$(top_srcdir)/src/Foundation

##-- barcode-graph
barcode_graph_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

barcode_graph_SOURCES = \
	barcode-graph.cc

##-- build-persistant-bank
build_persistant_bank_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

build_persistant_bank_SOURCES = \
	build-persistant-bank.cc


##-- END OF MAKEFILE --##
