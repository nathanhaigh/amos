##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	casm-layout \
	casm-breaks

noinst_PROGRAMS = \
	casm-libsize


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS


#-- casm-layout
casm_layout_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
casm_layout_SOURCES = \
	casm-layout.cc


#-- casm-breaks
casm_breaks_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
casm_breaks_SOURCES = \
	casm-breaks.cc


#-- casm-libsize
casm_libsize_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
casm_libsize_SOURCES = \
	casm-libsize.cc

##-- END OF MAKEFILE --##