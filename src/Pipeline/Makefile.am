##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk


##-- TO BE INSTALLED
dist_bin_SCRIPTS =

if BUILD_all
dist_bin_SCRIPTS += \
	AMOScmp.acf \
	AMOSmini.acf
endif BUILD_all

if BUILD_AMOScmp
dist_bin_SCRIPTS += AMOScmp.acf
endif BUILD_AMOScmp

if BUILD_AMOSmini
dist_bin_SCRIPTS += AMOSmini.acf
endif BUILD_AMOSmini


##-- END OF MAKEFILE --##