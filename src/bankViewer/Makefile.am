##-- MAKE FRAGMENT FOR AMOS RULES
include $(top_srcdir)/config/amos.mk

bin_PROGRAMS = hawkeye

AM_CPPFLAGS = \
    $(QT_CXXFLAGS)  \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/Slice  \
	-I$(top_srcdir)/src/AMOS   \
	-I$(top_srcdir)/src/Staden/read \
    -I$(top_srcdir)/src/Contig

%.moc.cc: %.hh
	$(QT_MOC) -o $@ $<

hawkeye_MOCSOURCES = \
        AssemblyStats.cc       \
        BufferedLineEdit.cc    \
        CGraphContig.cc        \
        CGraphEdge.cc          \
        CGraphView.cc          \
        CGraphWindow.cc        \
        ChromoField.cc         \
        ChromoPicker.cc        \
        ConsensusField.cc      \
        ContigCanvasItem.cc    \
        ContigPicker.cc        \
        CoverageCanvasItem.cc  \
        CoverageRectCanvasItem.cc \
        DetailWidget.cc        \
        FeatureBrowser.cc      \
        FeatureCanvasItem.cc   \
        HistogramWidget.cc     \
        HistogramWindow.cc     \
        InsertCanvasItem.cc    \
        InsertField.cc         \
        InsertPosition.cc      \
        InsertWidget.cc        \
        InsertWindow.cc        \
        LaunchPad.cc           \
        LaunchPadBase.cc       \
        LibraryPicker.cc       \
        MainWindow.cc          \
        NChartWindow.cc        \
        NChartWindowBase.cc    \
        NChartWidget.cc        \
        NetworkCom.cc          \
        OverviewField.cc       \
        QueryWidget.cc         \
        RangeScrollBar.cc      \
        ReadInfo.cc            \
        ReadPicker.cc          \
        RenderSeq.cc           \
        ScaffoldPicker.cc      \
        SelectionWidget.cc     \
        SplashScreen.cc        \
        TilingField.cc         \
        TilingFrame.cc

nodist_hawkeye_SOURCES = $(hawkeye_MOCSOURCES:.cc=.moc.cc)

hawkeyeHeaders = $(hawkeye_MOCSOURCES:.cc=.hh) \
                    AlignmentInfo.hh 

BUILT_SOURCES = $(hawkeye_MOCSOURCES:.cc=.moc.cc)
CLEANFILES = $(hawkeye_MOCSOURCES:.cc=.moc.cc)

hawkeye_SOURCES =           \
        main.cc                \
        ChromoStore.cc \
        ChromoStore.hh \
	UIElements.cc          \
	UIElements.hh          \
        NChartStats.cc         \
        NChartStats.hh         \
        $(hawkeye_MOCSOURCES)

EXTRA_DIST = \
        hawkeye.pro \
        $(hawkeyeHeaders) \
        icons/AAI.xpm \
        icons/fontdecrease.xpm \
        icons/fontincrease.xpm \
        icons/rangearrow_down.png \
        icons/rangearrow_left.png \
        icons/rangearrow_right.png \
        icons/rangearrow_up.png \
        icons/rangebar_horiz.png \
        icons/rangebar_vert.png \
        icons/rangegrip_down.png \
        icons/rangegrip_left.png \
        icons/rangegrip_right.png \
        icons/rangegrip_up.png \
        icons/rangegroove_horiz.png \
        icons/rangegroove_vert.png \
        icons/rangearrow_down.xpm \
        icons/rangearrow_left.xpm \
        icons/rangearrow_right.xpm \
        icons/rangearrow_up.xpm \
        icons/rangebar_horiz.xpm \
        icons/rangebar_vert.xpm \
        icons/rangegrip_down.xpm \
        icons/rangegrip_left.xpm \
        icons/rangegrip_right.xpm \
        icons/rangegrip_up.xpm \
        icons/rangegroove_horiz.xpm \
        icons/rangegroove_vert.xpm \
        icons/pointer_tool.xpm \
        icons/zoom_in.xpm \
        icons/zoom_out.xpm \
        icons/Hawkeye.xpm \
        icons/Hawkeye.jpg


hawkeye_LDADD = $(QT_LIBS) $(EXTRA_LDADD) \
                $(top_builddir)/src/Contig/libDataStore.a \
                $(top_builddir)/src/Common/libCommon.a   \
                $(top_builddir)/src/AMOS/libAMOS.a       \
                $(top_builddir)/src/Slice/libSlice.a     \
                $(top_builddir)/src/Staden/read/libread.a


