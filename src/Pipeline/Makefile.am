##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
dist_bin_SCRIPTS =

if BUILD_all
dist_bin_SCRIPTS += \
	AMOScmp.acf \
	minimus.acf \
	assembleRNAs.acf \
	runCA.clean.acf \
	runCA.meta.acf \
	runCA.prok.acf \
	runCA.euk.acf \
	runCA.stub.acf
endif BUILD_all

if BUILD_AMOScmp
dist_bin_SCRIPTS += AMOScmp.acf
endif BUILD_AMOScmp

if BUILD_minimus
dist_bin_SCRIPTS += minimus.acf
endif BUILD_minimus


##-- END OF MAKEFILE --##
