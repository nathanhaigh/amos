#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_ 1

#include <qmainwindow.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qscrollbar.h>
#include <qpopupmenu.h>
#include <string>

#include "TilingFrame.hh"
#include "ContigPicker.hh"
#include "ReadPicker.hh"
#include "FeatureBrowser.hh"
#include "LibraryPicker.hh"
#include "ChromoPicker.hh"
#include "DataStore.hh"
#include "InsertWindow.hh"
#include "CGraphWindow.hh"

class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent=0, const char *name=0 );

  void setBankname(std::string bankname);

public slots:
  void chooseBank();
  void chooseContig();

  void addChromoPath(const QString &);
  void addChromoDB(const QString &);
  void setChromoDB(const QString &);
  void setGindexRange(int, int);
  void setContigId(int contigID);
  void setGindex(int gindex);

  void showInserts();
  void showCGraph();

  void toggleShowPositions();
  void toggleShowIndicator();
  void toggleHighlightDiscrepancy();
  void toggleSNPColoring();
  void toggleDisplayAllChromo();
  void toggleLowQualityLowerCase();
  void toggleBaseColors();
  void toggleShowFullRange();
  void toggleDisplayQV();

  void fontIncrease();
  void fontDecrease();

  void showReadPicker();
  void showLibPicker();
  void showChromoPicker();
  void showFeatureBrowser();
  
  void jumpFGindex();
  void jumpPGindex();

signals:
  void bankSelected(std::string bankname);
  void contigIdSelected(int contigId);
  void gindexChanged(int gindex);
  void highlightRead(int iid);
  void chromoDBSet(const QString & db);

  void toggleShowFullRange(bool);
  void toggleShowNumbers(bool);
  void toggleShowIndicator(bool);
  void toggleDisplayQV(bool);
  void toggleLowQualityLowerCase(bool);
  void toggleBaseColors(bool);
  void toggleSNPColoring(bool);
  void toggleHighlightDiscrepancy(bool);
  void toggleDisplayAllChromo(bool);
  void setFontSize(int);

private:
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

  QPopupMenu * m_options;

  int m_posid;
  int m_qvid;
  int m_indicatorid;
  int m_highid;
  int m_prefetch;
  int m_fontsize;
  int m_lowquallowerid;
  int m_basecolorid;
  int m_showfullid;
  int m_snpcoloringid;

  int m_gindex;

  DataStore m_datastore;
};

#endif
