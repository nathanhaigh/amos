bin_PROGRAMS = bankViewer \
               cestat-cov \
               insert-sizes \
               library-histogram \
               list-linked-contigs

%.moc.cc: %.hh
	$(QT_MOC) -o $@ $<

bankViewer_MOCSOURCES = \
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
        CoverageStats.cc       \
        DataStore.cc           \
	    DetailWidget.cc        \
        FeatureBrowser.cc      \
        FeatureCanvasItem.cc   \
        HistogramWidget.cc     \
        HistogramWindow.cc     \
        InsertCanvasItem.cc    \
        Insert.cc              \
        InsertField.cc         \
        InsertPosition.cc      \
        InsertStats.cc         \
        InsertWidget.cc        \
        InsertWindow.cc        \
	    LaunchPad.cc           \
    	LaunchPadBase.cc           \
        LibraryPicker.cc       \
        MainWindow.cc          \
        NChartWindow.cc        \
        NChartWidget.cc        \
        NetworkCom.cc          \
	    OverviewField.cc       \
    	QueryWidget.cc         \
        RangeScrollBar.cc      \
        ReadInfo.cc            \
        ReadPicker.cc          \
        RenderSeq.cc           \
        ScaffoldPicker.cc      \
        SplashScreen.cc        \
        TilingField.cc         \
        TilingFrame.cc         \
        UIElements.cc          \
        krangeslider.cc        \
        birangecontrol.cc       

nodist_bankViewer_SOURCES = $(bankViewer_MOCSOURCES:.cc=.moc.cc)

bankViewerHeaders = $(bankViewer_MOCSOURCES:.cc=.hh) AlignmentInfo.hh

BUILT_SOURCES = $(bankViewer_MOCSOURCES:.cc=.moc.cc)
CLEANFILES = $(bankViewer_MOCSOURCES:.cc=.moc.cc)

bankViewer_SOURCES =             \
        main.cc                  \
        $(bankViewer_MOCSOURCES)

EXTRA_DIST = \
       bankViewer.pro \
       $(bankViewerHeaders) \
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
       icons/rangegroove_vert.png  \
       ADDICONS


bankViewer_LDADD = $(QT_LIBS) $(EXTRA_LDADD) \
                   $(top_builddir)/src/Common/libCommon.a   \
                   $(top_builddir)/src/AMOS/libAMOS.a       \
                   $(top_builddir)/src/Slice/libSlice.a     \
                   $(top_builddir)/src/Staden/read/libread.a

AM_CPPFLAGS = \
    $(QT_CXXFLAGS)  \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/Slice  \
	-I$(top_srcdir)/src/AMOS   \
	-I$(top_srcdir)/src/Staden/read

cestat_cov_SOURCES = \
  cestat-cov.cc \
  Insert.cc \
  DataStore.cc \
  CoverageStats.cc

cestat_cov_LDADD = $(QT_LIBS) $(EXTRA_LDADD) \
                   $(top_builddir)/src/Common/libCommon.a   \
                   $(top_builddir)/src/AMOS/libAMOS.a      \
                   $(top_builddir)/src/Staden/read/libread.a

insert_sizes_SOURCES = \
  insert-sizes.cc \
  Insert.cc \
  DataStore.cc \
  InsertStats.cc 

insert_sizes_LDADD = $(QT_LIBS) $(EXTRA_LDADD) \
                     $(top_builddir)/src/Common/libCommon.a   \
                     $(top_builddir)/src/AMOS/libAMOS.a      \
                     $(top_builddir)/src/Staden/read/libread.a

library_histogram_SOURCES = \
  library-histogram.cc \
  Insert.cc \
  DataStore.cc \
  InsertStats.cc

library_histogram_LDADD = $(QT_LIBS) $(EXTRA_LDADD) \
                     $(top_builddir)/src/Common/libCommon.a   \
                     $(top_builddir)/src/AMOS/libAMOS.a      \
                     $(top_builddir)/src/Staden/read/libread.a

list_linked_contigs_SOURCES = \
  list-linked-contigs.cc \
  Insert.cc \
  DataStore.cc

list_linked_contigs_LDADD = $(QT_LIBS) $(EXTRA_LDADD) \
                     $(top_builddir)/src/Common/libCommon.a   \
                     $(top_builddir)/src/AMOS/libAMOS.a      \
                     $(top_builddir)/src/Staden/read/libread.a
