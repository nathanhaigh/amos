#ifndef LAUNCHPAD_H
#define LAUNCHPAD_H

#include <string>

#include <qcursor.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlistview.h>

#include "LaunchPadBase.hh"
#include "NetworkCom.hh"
#include "DataStore.hh"

class InsertWindow;
class MainWindow;
class NChartStats;

class LaunchPad : public LaunchPadBase
{
  Q_OBJECT

public:
  LaunchPad(QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel);
  ~LaunchPad();

public slots: 

  // global
  void fileOpen();
  void fileExit();
  void initDisplay();

  void setBankname(std::string bankname);

  void showAll();
  void showInserts();
  void showTiling();

  // dsommer: added import
  void fileImport();

  void fileChromoPaths();
  void addChromoPath(const QString & path);
  void setChromoDB(const QString & db);

  void loadKmers(std::string file);

  void newConnect(ClientSocket * s);
  void setContigLocation(QString, int);
  void jumpToRead(int iid);
  void initializeSimpleServer(int port);
  void enableTraceFetch(bool dofetch);

  void setScaffoldId(int);
  void setContigId(int);
  void setGindex(int);


  // stats tab
  void loadAssemblyStatistics();
  void loadNCharts();

  // features tab
  void initFeatures();
  void loadFeatures();
  void featureSelected(QListViewItem *);
  void featureViewButtonSelected();
  void featureGroupChanged(int);

  // libraries tab
  void initLibraries();
  void loadLibraries();
  void librarySelected(QListViewItem *);
  void libraryViewSelected();
  void libraryClearLengthSelected();
  void libraryReadLengthSelected();
  void libraryGCSelected();

  // scaffolds tab
  void initScaffolds();
  void loadScaffolds();
  void scaffoldSelected(QListViewItem *);
  void scaffoldSelectIID(const QString &);
  void scaffoldSelectEID(const QString &);
  void scaffoldViewSelected();
  void scaffoldSpanHistogram();
  void scaffoldContigHistogram();

  // contigs tab
  void initContigs();
  void loadContigs();
  void contigSelected(QListViewItem *);
  void contigSelectIID(const QString &);
  void contigSelectEID(const QString &);
  void contigViewSelected();
  void contigLengthHistogram();
  void contigReadCountHistogram();
  void contigGCHistogram();


  // reads tab
  void initReads();
  void loadReads();
  void readSelected(QListViewItem *);
  void readSelectIID(const QString &);
  void readSelectEID(const QString &);
  void readViewSelected();
  void readLengthHistogram();
  void readGCHistogram();


signals:
  void setTilingVisibleRange(int,int,int);
  void bankSelected();
  void highlightRead(int);

  void contigIdSelected(int);
  void gindexSelected(int);


private:

  DataStore * m_datastore;

  InsertWindow * m_insertWindow;
  MainWindow * m_tilingWindow;
  QMainWindow * m_chromoPicker;
  NChartStats * m_scaffstats;
  NChartStats * m_contigstats;

  int m_gindex;
  int m_verbose;

};

#endif
