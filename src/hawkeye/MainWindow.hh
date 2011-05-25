#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_ 1

#include <q3mainwindow.h>
#include <qspinbox.h>
#include <qscrollbar.h>
#include <q3popupmenu.h>
#include <qlineedit.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QLabel>
#include <string>
#include <map>


class TilingFrame;
class QLabel;
class Q3VBox;
class DataStore;

class MainWindow: public Q3MainWindow
{
  Q_OBJECT

public:
  MainWindow(DataStore * datastore, QWidget *parent=0, const char *name=0 );


public slots:
  void setGindexRange(int, int);
  void setGindex(int gindex);
  void setContigId(int);
  void setReadnameWidth();

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
  void togglePackReads();

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
  void togglePackReads(bool);
  void setFontSize(int);


  void advanceNextDiscrepancy();
  void advancePrevDiscrepancy();

private:
  void initializeTiling(TilingFrame * tiling, bool isReference);

  QSpinBox * m_contigspin;
  QSpinBox * m_gspin;
  QScrollBar * m_slider;
  
  TilingFrame * m_tiling;

  QWidget * m_parent;
  Q3PopupMenu * m_options;
  QLineEdit * m_searchedit;
  QLabel * m_offsetlabel;

  int m_packid;              bool m_packchecked;
  int m_qvid;                bool m_qvchecked;
  int m_cqvid;               bool m_cqvchecked;
  int m_highid;              bool m_highchecked;
  int m_prefetch;            bool m_prefetchchecked;
  int m_fontsize;            
  int m_lowquallowerid;      bool m_lowquallowerchecked;
  int m_basecolorid;         bool m_basecolorchecked;
  int m_namewidthid;         
  int m_showfullid;          bool m_showfullchecked;
  int m_snpcoloringid;       bool m_snpcoloringchecked;
  int m_polyid;              bool m_polychecked;
  int m_qvcolorid;           bool m_qvcolorchecked;
  int m_ungappedid;          bool m_ungappedchecked;
  int m_contigid;       

  int m_gindex;
  DataStore * m_datastore;

  Q3VBox * m_outervbox;
  QWidget * m_multiTilingWidget;
};

#endif
