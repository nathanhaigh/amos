#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_ 1

#include <qmainwindow.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qscrollbar.h>
#include <qmainwindow.h>
#include <string>
#include <qpopupmenu.h>

#include "TilingFrame.hh"
#include "ContigPicker.hh"

class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent=0, const char *name=0 );

  void setBankname(std::string bankname);
  void setContigId(int contigID);
  void setGindex(int gindex);

public slots:
  void chooseBank();
  void chooseContig();
  void setContigRange(int, int);
  void setGindexRange(int, int);
  void contigSelected(int);

  void showInserts();

  void toggleShowPositions();
  void toggleShowQV();
  void toggleHighlightDiscrepancy();
  void togglePrefetchChromatograms();

  void fontIncrease();
  void fontDecrease();

signals:
  void bankSelected(std::string bankname);
  void contigIdSelected(int contigId);
  void gindexChanged(int gindex);

private:
  QSpinBox * m_contigid;
  QSpinBox * m_gindex;
  QScrollBar * m_slider;
  QMainWindow * m_contigPicker;

  QPopupMenu * m_options;
  int m_posid;
  int m_qvid;
  int m_highid;
  int m_prefetch;
  int m_fontsize;

  std::string m_bankname;
  TilingFrame * m_tiling;
};

#endif
