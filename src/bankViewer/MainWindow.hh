#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_ 1

#include <qmainwindow.h>
#include <qspinbox.h>
#include <qscrollbar.h>
#include <qpopupmenu.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <string>
#include <map>


class TilingFrame;
class QLabel;
class QVBox;
class DataStore;

class MainWindow: public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(DataStore * datastore, QWidget *parent=0, const char *name=0 );


public slots:
  void setGindexRange(int, int);
  void setGindex(int gindex);
  void setContigId(int);

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

  void bankChanged();

  void fontIncrease();
  void fontDecrease();
  
  void jumpFGindex();
  void jumpPGindex();

  void findNext();
  void findPrev();

  void updateGSpin(int gindex);
  void loadGSpin(int pos);


signals:
  void contigIdSelected(int);
  void gindexChanged(int gindex);
  void highlightRead(int iid);
  void searchString(const QString & str, bool forward);
  void setTilingVisibleRange(int, int, int);

  void toggleShowFullRange(bool);
  void toggleShowNumbers(bool);
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

  QSpinBox * m_contigspin;
  QSpinBox * m_gspin;
  QScrollBar * m_slider;
  
  TilingFrame * m_tiling;

  QPopupMenu * m_options;
  QLineEdit * m_searchedit;
  QLabel * m_offsetlabel;

  int m_qvid;
  int m_cqvid;
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
  int m_contigid;

  int m_gindex;
  DataStore * m_datastore;

  QVBox * m_outervbox;
  QWidget * m_multiTilingWidget;
};

#endif
