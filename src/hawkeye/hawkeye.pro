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
ChromoStore.cc\
ConsensusField.cc\
ContigCanvasItem.cc\
ContigPicker.cc\
CoverageCanvasItem.cc\
CoverageRectCanvasItem.cc\
DetailWidget.cc\
FeatureBrowser.cc\
FeatureCanvasItem.cc\
HistogramWidget.cc\
HistogramWindow.cc\
InsertCanvasItem.cc\
InsertField.cc\
InsertPosition.cc\
InsertWidget.cc\
InsertWindow.cc\
LaunchPadBase.cc\
LaunchPad.cc\
LibraryPicker.cc\
main.cc\
MainWindow.cc\
NChartStats.cc\
NChartWidget.cc\
NChartWindowBase.cc\
NChartWindow.cc\
NetworkCom.cc\
OverviewField.cc\
QueryWidget.cc\
RangeScrollBar.cc\
ReadInfo.cc\
ReadPicker.cc\
RenderSeq.cc\
ScaffoldPicker.cc\
SelectionWidget.cc\
SplashScreen.cc\
TilingField.cc\
TilingFrame.cc\
UIElements.cc


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
ChromoStore.hh\
ConsensusField.hh\
ContigCanvasItem.hh\
ContigPicker.hh\
CoverageCanvasItem.hh\
CoverageRectCanvasItem.hh\
DetailWidget.hh\
FeatureBrowser.hh\
FeatureCanvasItem.hh\
HistogramWidget.hh\
HistogramWindow.hh\
InsertCanvasItem.hh\
InsertField.hh\
InsertPosition.hh\
InsertWidget.hh\
InsertWindow.hh\
LaunchPadBase.hh\
LaunchPad.hh\
LibraryPicker.hh\
MainWindow.hh\
NChartStats.hh\
NChartWidget.hh\
NChartWindowBase.hh\
NChartWindow.hh\
NetworkCom.hh\
OverviewField.hh\
QueryWidget.hh\
RangeScrollBar.hh\
ReadInfo.hh\
ReadPicker.hh\
RenderSeq.hh\
ScaffoldPicker.hh\
SelectionWidget.hh\
SplashScreen.hh\
TilingField.hh\
TilingFrame.hh\
UIElements.hh


INCLUDEPATH	+= ../AMOS ../Common ../Slice ../Staden/read ../../ ../Contig
LIBS	+=  -L../AMOS -L../Common -L../Staden/read -L../Contig/ -lDataStore -lAMOS -lCommon -lAMOS -lCommon -lread -lz

#The following line was inserted by qt3to4
QT +=  qt3support 

#CONFIG += x86_64
