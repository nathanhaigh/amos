##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

noinst_LIBRARIES = \
	libread.a

noinst_HEADERS = \
	Read.h \
	abi.h \
	alf.h \
	array.h \
	calc_crc.h \
	compress.h \
	compression.h \
	error.h \
	expFileIO.h \
	fpoint.h \
	mach-io.h \
	misc.h \
	open_trace_file.h \
	os.h \
	plain.h \
	scf.h \
	scf_extras.h \
	seqIOABI.h \
	seqIOCTF.h \
	tar_format.h \
	traceType.h \
	translate.h \
	xalloc.h \
	ztr.h

libread_a_SOURCES = \
        Read.c \
        array.c \
        compress.c \
        ctfCompress.c \
        error.c \
        expFileIO.c \
        files.c \
        find.c \
        fpoint.c \
        mach-io.c \
        misc_scf.c \
        open_trace_file.c \
        read_alloc.c \
        read_scf.c \
        scf_extras.c \
        seqIOABI.c \
        seqIOALF.c \
        seqIOCTF.c \
        seqIOPlain.c \
        strings.c \
        traceType.c \
        translate.c \
        write_scf.c \
        xalloc.c

if BUILD_LIBZ
 libread_a_SOURCES += \
	compression.c \
        ztr.c \
        ztr_translate.c
endif



##-- END OF MAKEFILE --##