bin_PROGRAMS = bankViewer

%.moc.cc: %.hh
	$(QT_MOC) -o $@ $<

bankViewer_MOCSOURCES = \
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
        DataStore.cc           \
        FeatureBrowser.cc      \
        FeatureCanvasItem.cc   \
        InsertCanvasItem.cc    \
        Insert.cc              \
        InsertField.cc         \
        InsertPosition.cc      \
        InsertWidget.cc        \
        InsertWindow.cc        \
        LibraryPicker.cc       \
        MainWindow.cc          \
        NetworkCom.cc          \
        ReadInfo.cc            \
        ReadPicker.cc          \
        RenderSeq.cc           \
        ScaffoldPicker.cc      \
        TilingField.cc         \
        TilingFrame.cc         \
        UIElements.cc          

bankViewer_MOC = $(bankViewer_MOCSOURCES:.cc=.moc.cc)

bankViewer_SOURCES =             \
        main.cc                  \
        $(bankViewer_MOCSOURCES) \
        $(bankViewer_MOC)

BUILT_SOURCES = $(bankViewer_MOC)

bankViewer_LDADD = $(QT_LIBS) $(EXTRA_LDADD) \
                   $(top_builddir)/src/Common/libCommon.a   \
                   $(top_builddir)/src/AMOS/libAMOS.a       \
                   $(top_builddir)/src/Slice/libSlice.a     \
                   /local/devel/SE/IO_Lib/mschatz/SE/IO_Lib/install/${HOSTTYPE}/lib/libread.a

AM_CPPFLAGS = \
    $(QT_CXXFLAGS)  \
	-I$(top_srcdir)/src/Common \
	-I$(top_srcdir)/src/Slice  \
	-I$(top_srcdir)/src/AMOS   \
    -I/local/devel/SE/IO_Lib/mschatz/SE/IO_Lib/install/${HOSTTYPE}/include/io_lib/


CLEANFILES = $(BUILT_SOURCES)
