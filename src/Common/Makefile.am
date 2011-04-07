##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


#-- TO BE INSTALLED
amoslib_LIBRARIES = \
	libCommon.a

amosinclude_HEADERS = \
	alloc.hh \
	amp.hh \
	delcher.hh \
	delta.hh \
	fasta.hh \
	prob.hh \
	fastq.hh


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/AMOS


##-- libCommon.a
libCommon_a_SOURCES = \
	amp.cc \
	delcher.cc \
	delta.cc \
	fasta.cc \
	prob.cc  \
	fastq.cc


##-- END OF MAKEFILE --##