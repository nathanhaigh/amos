##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
dist_bin_SCRIPTS =

if BUILD_all
dist_bin_SCRIPTS += \
	AMOScmp.acf \
	AMOScmp-shortReads.acf \
	AMOScmp-shortReads-alignmentTrimmed.acf \
	minimus.acf \
	minimus2.acf \
	assembleRNAs.acf \
	Minimo.acf
endif BUILD_all

if BUILD_AMOScmp
dist_bin_SCRIPTS += AMOScmp.acf
dist_bin_SCRIPTS += AMOScmp-shortReads.acf
dist_bin_SCRIPTS += AMOScmp-shortReads-alignmentTrimmed.acf
endif BUILD_AMOScmp

if BUILD_minimus
dist_bin_SCRIPTS += minimus.acf
dist_bin_SCRIPTS += minimus2.acf
endif BUILD_minimus


##-- END OF MAKEFILE --##
