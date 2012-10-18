##-- SUBDIRS TO RECURSE
SUBDIRS =

if BUILD_all
SUBDIRS += \
	GNU \
	AMOS \
	Common \
    Foundation \
	Graph \
	Slice \
	CelMsg \
	Message \
	Bank \
	Align \
	Tigger \
	Compare \
	Bambus \
	Casm \
	CtgTrim \
	Sim \
	Staden \
    Contig \
	Validation \
	Converters \
	Utils \
	PerlModules \
    PythonModules \
	Pipeline

if BUILD_QT4
SUBDIRS += hawkeye
endif BUILD_QT4

if BUILD_Experimental
SUBDIRS += Experimental
endif BUILD_Experimental

endif BUILD_all

if BUILD_AMOScmp
SUBDIRS += \
	GNU \
	AMOS \
	Common \
	Slice \
	CelMsg \
	Message \
	Bank \
	Align \
	Casm \
	Converters \
	Utils \
	PerlModules \
	Pipeline
endif BUILD_AMOScmp

if BUILD_minimus
SUBDIRS += \
	GNU \
	AMOS \
	Common \
	Graph \
	Slice \
	CelMsg \
	Message \
	Bank \
	Align \
        Tigger \
	Converters \
	Utils \
	PerlModules \
	Pipeline
endif BUILD_minimus


##-- END OF MAKEFILE --##
