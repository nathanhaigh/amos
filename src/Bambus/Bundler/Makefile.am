##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	Bundler

dist_bin_SCRIPTS = \
	Chainer.pl \
	Joiner.pl


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS


##-- Bundler
Bundler_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
Bundler_SOURCES = \
	Bundler.cc \
	Bundler.hh


##-- END OF MAKEFILE --##