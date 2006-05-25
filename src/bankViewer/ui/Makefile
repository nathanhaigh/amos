all: ../DetailWidget.cc \
     ../DetailWidget.hh \
     ../LaunchPadBase.cc \
     ../LaunchPadBase.hh \
     ../NChartWindowBase.cc \
     ../NChartWindowBase.hh \
     ../QueryWidget.cc \
     ../QueryWidget.hh \
     ../SelectionWidget.cc \
     ../SelectionWidget.hh


UIC=uic



UMIACSUICPATH=/fs/sz-user-supported/Linux-i686/packages/Qt-3.3.6/bin/uic
UMIACSUIC=$(shell ls $(UMIACSUICPATH))

ifeq ($(UMIACSUIC), $(UMIACSUICPATH))
  UIC=$(UMIACSUICPATH)
endif

../%.hh: %.ui
	@ echo "+ Writing $@"
	@ ${UIC} $< | grep -v '\*\*' > $@


../%.cc: ../%.hh %.ui
	@ echo "+ Writing $@"
	@ ${UIC} -impl `echo $^ | sed s^\.\./^^`  | grep -v '\*\*' > $@
