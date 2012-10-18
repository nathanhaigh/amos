##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
amoslib_LIBRARIES = \
	libSlice.a

amosinclude_HEADERS = \
	Slice.h


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/AMOS


##-- libSlice.a
libSlice_a_SOURCES = \
	getConsQC.c \
	getConsQV.c \
	newmem.c \
	utility.c \
	version.h


##-- END OF MAKEFILE --##