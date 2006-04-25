CONFIG += thread

SOURCES	+= \
AssemblyStats.cc\
BufferedLineEdit.cc\
CGraphContig.cc\
CGraphEdge.cc\
CGraphView.cc\
CGraphWindow.cc\
ChromoField.cc\
ChromoPicker.cc\
ConsensusField.cc\
ContigCanvasItem.cc\
ContigPicker.cc\
CoverageCanvasItem.cc\
CoverageRectCanvasItem.cc
CoverageStats.cc\
DataStore.cc\
DetailWidget.cc\
FeatureBrowser.cc\
FeatureCanvasItem.cc\
HistogramWidget.cc\
HistogramWindow.cc\
Insert.cc\
InsertCanvasItem.cc\
InsertField.cc\
InsertPosition.cc\
InsertStats.cc\
InsertWidget.cc\
InsertWindow.cc\
LibraryPicker.cc\
MainWindow.cc\
NetworkCom.cc\
OverviewField.cc\
OverviewWidget.cc\
OverviewWindow.cc\
QueryWidget.cc\
ReadInfo.cc\
ReadPicker.cc\
RenderSeq.cc\
ScaffoldPicker.cc\
SplashScreen.cc\
TilingField.cc\
TilingFrame.cc\
UIElements.cc\
main.cc

HEADERS	+= \
AlignmentInfo.hh\
AssemblyStats.hh\
BufferedLineEdit.hh\
CGraphContig.hh\
CGraphEdge.hh\
CGraphView.hh\
CGraphWindow.hh\
ChromoField.hh\
ChromoPicker.hh\
ConsensusField.hh\
ContigCanvasItem.hh\
ContigPicker.hh\
CoverageCanvasItem.hh\
CoverageRectCanvasItem.hh\
CoverageStats.hh\
DataStore.hh\
DetailWidget.hh\
FeatureBrowser.hh\
FeatureCanvasItem.hh\
HistogramWidget.hh\
HistogramWindow.hh\
Insert.hh\
InsertCanvasItem.hh\
InsertField.hh\
InsertPosition.hh\
InsertStats.hh\
InsertWidget.hh\
InsertWindow.hh\
LibraryPicker.hh\
MainWindow.hh\
NetworkCom.hh\
OverviewField.hh\
OverviewWidget.hh\
OverviewWindow.hh\
QueryWidget.hh\
ReadInfo.hh\
ReadPicker.hh\
RenderSeq.hh\
ScaffoldPicker.hh\
SplashScreen.hh\
TilingField.hh\
TilingFrame.hh\
UIElements.hh


INCLUDEPATH	+= ../AMOS ../Common ../Slice ../Staden/read ../../
LIBS	+=  -L../AMOS -L../Common -L../Staden/read -lAMOS -lCommon -lAMOS -lCommon -lread -lz

