#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_ 1

#include <qmainwindow.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qscrollbar.h>
#include <qpopupmenu.h>
#include <qlineedit.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <string>
#include <map>

#include "NetworkCom.hh"
#include "DataStore.hh"

class TilingFrame;
class ContigPicker;
class ScaffoldPicker;
class ReadPicker;
class FeatureBrowser;
class LibraryPicker;
class ChromoPicker;
class InsertWindow;
class CGraphWindow;
class OverviewWindow;
class QLabel;



class ReferenceAlignment
{
public:
  ReferenceAlignment() {}
  ~ReferenceAlignment() {}

  std::string m_reference;
  std::string m_query;

  int m_rstart;
  int m_rend;

  int m_qstart;
  int m_qend;

  int m_chain;

  float m_percentid;
};

class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent=0, const char *name=0 );

  void setBankname(std::string bankname);
  void loadKmers(std::string file);

public slots:
  void chooseBank();
  void chooseContig();
  void newConnect(ClientSocket * s);
  void setContigLocation(QString, int);
  void jumpToRead(int iid);
  void initializeSimpleServer(int port);
  void enableTraceFetch(bool dofetch);

  void addChromoPath(const QString &);
  void addChromoDB(const QString &);
  void setChromoDB(const QString &);
  void setGindexRange(int, int);
  void setContigId(int contigID);
  void setGindex(int gindex);

  void showInserts();
  void showCGraph();
  void showOverview();

  void toggleShowPositions();
  void toggleShowIndicator();
  void toggleHighlightDiscrepancy();
  void toggleSNPColoring();
  void toggleDisplayAllChromo();
  void toggleLowQualityLowerCase();
  void toggleBaseColors();
  void toggleQVColoring();
  void toggleShowFullRange();
  void toggleDisplayQV();
  void toggleShowConsQV();
  void togglePolymorphismView();
  void toggleUngapped();

  void fontIncrease();
  void fontDecrease();

  void showAssemblyStats();
  void showReadPicker();
  void showLibPicker();
  void showChromoPicker();
  void showFeatureBrowser();
  void showScaffoldPicker();
  
  void jumpFGindex();
  void jumpPGindex();

  void findNext();
  void findPrev();

  void updateGSpin(int gindex);
  void loadGSpin(int pos);


signals:
  void bankSelected();
  void contigIdSelected(int contigId);
  void gindexChanged(int gindex);
  void highlightRead(int iid);
  void chromoDBSet(const QString & db);
  void searchString(const QString & str, bool forward);

  void toggleShowFullRange(bool);
  void toggleShowNumbers(bool);
  void toggleShowIndicator(bool);
  void toggleDisplayQV(bool);
  void toggleLowQualityLowerCase(bool);
  void toggleShowConsQV(bool);
  void toggleBaseColors(bool);
  void toggleSNPColoring(bool);
  void toggleHighlightDiscrepancy(bool);
  void toggleDisplayAllChromo(bool);
  void togglePolymorphismView(bool);
  void toggleQVColoring(bool);
  void toggleShowUngapped(bool);
  void setFontSize(int);

  void advanceNextDiscrepancy();
  void advancePrevDiscrepancy();

private:
  void initializeTiling(TilingFrame * tiling, bool isReference);

  QSpinBox * m_contigid;
  QSpinBox * m_gspin;
  QScrollBar * m_slider;
  QMainWindow * m_contigPicker;
  QMainWindow * m_readPicker;
  QMainWindow * m_libPicker;
  QMainWindow * m_chromoPicker;
  QMainWindow * m_featPicker;
  InsertWindow * m_insertWindow;
  TilingFrame * m_tiling;
  CGraphWindow * m_cgraphWindow;
  OverviewWindow * m_overviewWindow;

  QPopupMenu * m_options;
  QLineEdit * m_searchedit;
  QLabel * m_offsetlabel;

  int m_posid;
  int m_qvid;
  int m_cqvid;
  int m_indicatorid;
  int m_highid;
  int m_prefetch;
  int m_fontsize;
  int m_lowquallowerid;
  int m_basecolorid;
  int m_showfullid;
  int m_snpcoloringid;
  int m_polyid;
  int m_qvcolorid;
  int m_ungappedid;

  int m_gindex;
  DataStore m_datastore;

  QVBox * m_outervbox;
  QWidget * m_multiTilingWidget;

  typedef std::multimap<std::string, ReferenceAlignment> ReferenceAlignments;

  ReferenceAlignments m_referenceAlignments;
  DataStore m_querystore;
};

#endif
