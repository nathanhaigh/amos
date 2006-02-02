##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

##-- TO BE INSTALLED
bin_PROGRAMS = \
    barcode-graph \
    build-persistent-bank \
    persistent-read-dist

dist_bin_SCRIPTS = \
    persistent-assembly.pl

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

##-- build-persistent-bank
build_persistent_bank_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

build_persistent_bank_SOURCES = \
	build-persistent-bank.cc

##-- persistent_read_dist
persistent_read_dist_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a \
	$(top_builddir)/src/Foundation/libAMOSFoundation.a

persistent_read_dist_SOURCES = \
	persistent-read-dist.cc

##-- END OF MAKEFILE --##
