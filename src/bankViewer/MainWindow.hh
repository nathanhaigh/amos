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

  void setDB(const QString &);
  void setGindexRange(int, int);
  void setContigId(int contigID);
  void setGindex(int gindex);

  void showInserts();
  void showCGraph();

  void toggleShowPositions();
  void toggleShowQV();
  void toggleHighlightDiscrepancy();
  void togglePrefetchChromatograms();
  void toggleLowQualityLowerCase();
  void toggleBaseColors();
  void toggleShowFullRange();

  void fontIncrease();
  void fontDecrease();

signals:
  void bankSelected(std::string bankname);
  void contigIdSelected(int contigId);
  void gindexChanged(int gindex);

private:
  QSpinBox * m_contigid;
  QSpinBox * m_gspin;
  QScrollBar * m_slider;
  QMainWindow * m_contigPicker;
  InsertWindow * m_insertWindow;
  TilingFrame * m_tiling;
  CGraphWindow * m_cgraphWindow;

  QPopupMenu * m_options;
  int m_posid;
  int m_qvid;
  int m_highid;
  int m_prefetch;
  int m_fontsize;
  int m_lowquallowerid;
  int m_basecolorid;
  int m_showfullid;

  int m_gindex;

  DataStore m_datastore;
};

#endif
