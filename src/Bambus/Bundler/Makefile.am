##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	Bundler \
	clk

dist_bin_SCRIPTS = \
	Chainer.pl \
	Joiner.pl


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS


##-- Bundler\
Bundler_CPPFLAGS = \
	-I$(top_builddir)/src/GNU
Bundler_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
Bundler_SOURCES = \
	Bundler.cc \
	Bundler.hh


##-- CLK
clk_CPPFLAGS = \
	-I$(top_builddir)/src/GNU
clk_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a \
	$(top_builddir)/src/GNU/libGNU.a
clk_SOURCES = \
	clk.cc

##-- END OF MAKEFILE --##