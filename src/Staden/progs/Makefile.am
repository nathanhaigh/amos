##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk
 
bin_PROGRAMS = \
	trace_comment \
	trace_comments \
	trace_convert \
	trace_seq \
    trace_scf_dump

AM_CPPFLAGS = \
        -I$(top_srcdir)/src/Staden/read

trace_comment_LDADD = \
	$(top_builddir)/src/Staden/read/libread.a
trace_comment_SOURCES = \
	trace_comment.c

trace_comments_LDADD = \
	$(top_builddir)/src/Staden/read/libread.a
trace_comments_SOURCES = \
	trace_comments.c

trace_convert_LDADD = \
	$(top_builddir)/src/Staden/read/libread.a
trace_convert_SOURCES = \
	trace_convert.c

trace_seq_LDADD = \
	$(top_builddir)/src/Staden/read/libread.a
trace_seq_SOURCES = \
	trace_seq.c

trace_scf_dump_LDADD = \
	$(top_builddir)/src/Staden/read/libread.a
trace_scf_dump_SOURCES = \
	trace_scf_dump.c

##-- END OF MAKEFILE --##
