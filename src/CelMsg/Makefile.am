##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
amoslib_LIBRARIES = \
	libCelMsg.a

amosinclude_HEADERS = \
	CelMsg.hh


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS


##-- libCelMsg.a
libCelMsg_a_SOURCES = \
	CelMsg.cc


##-- END OF MAKEFILE --##