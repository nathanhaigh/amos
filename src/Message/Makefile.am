##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
bin_PROGRAMS = \
	message-count \
	message-extract \
	message-validate


##-- GLOBAL INCLUDE
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/AMOS


##-- message-count
message_count_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
message_count_SOURCES = \
	message-count.cc

##-- message-extract
message_extract_LDADD = \
	$(top_builddir)/src/Common/libCommon.a \
	$(top_builddir)/src/AMOS/libAMOS.a
message_extract_SOURCES = \
	message-extract.cc

##-- message-validate
message_validate_LDADD = \
	$(top_builddir)/src/AMOS/libAMOS.a
message_validate_SOURCES = \
	message-validate.cc


##-- END OF MAKEFILE --##