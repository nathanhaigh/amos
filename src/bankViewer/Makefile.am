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

bankViewer_MOC = \
        BufferedLineEdit.moc.cc    \
        CGraphContig.moc.cc        \
        CGraphEdge.moc.cc          \
        CGraphView.moc.cc          \
        CGraphWindow.moc.cc        \
        ChromoField.moc.cc         \
        ChromoPicker.moc.cc        \
        ConsensusField.moc.cc      \
        ContigCanvasItem.moc.cc    \
        ContigPicker.moc.cc        \
        CoverageCanvasItem.moc.cc  \
        DataStore.moc.cc           \
        FeatureBrowser.moc.cc      \
        FeatureCanvasItem.moc.cc   \
        InsertCanvasItem.moc.cc    \
        Insert.moc.cc              \
        InsertField.moc.cc         \
        InsertPosition.moc.cc      \
        InsertWidget.moc.cc        \
        InsertWindow.moc.cc        \
        LibraryPicker.moc.cc       \
        MainWindow.moc.cc          \
        NetworkCom.moc.cc          \
        ReadInfo.moc.cc            \
        ReadPicker.moc.cc          \
        RenderSeq.moc.cc           \
        ScaffoldPicker.moc.cc      \
        TilingField.moc.cc         \
        TilingFrame.moc.cc         \
        UIElements.moc.cc          

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
