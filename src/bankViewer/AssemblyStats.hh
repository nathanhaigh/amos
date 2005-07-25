#ifndef ASSEMBLY_STATS_HH__
#define ASSEMBLY_STATS_HH__ 1

#include <qmainwindow.h>
#include <qstring.h>
#include <qtable.h>
#include "DataStore.hh"
#include <string>

using std::string;

class AssemblyStats : public QMainWindow
{
  Q_OBJECT

public:
  AssemblyStats(DataStore * datastore, QWidget * parent, const char * name);

public slots:
  void refreshTable();

private:
  void loadTable();
  void addRow(QString tag, QString value);
  void addRow(QString tag, int value);
  void addRow(QString tag, double value);

  int m_currow;

  DataStore * m_datastore;
  QTable * m_table;
};


#endif
