#include "AssemblyStats.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>

#include "foundation_AMOS.hh"

#include <vector>

using namespace std;


#define DIV(a,b) (b?(double(a)/b):0.0)

AssemblyStats::AssemblyStats(DataStore * datastore,
                       QWidget * parent, 
                       const char * name)
  :QMainWindow(parent, name)
{
  m_table = new QTable(100, 2, this);
  setCentralWidget(m_table);
  setCaption("Assembly Statistics");
  resize(550,500);
  show();

  m_table->setLeftMargin(0);
  m_table->verticalHeader()->setLabel(0, "Field");
  m_table->verticalHeader()->setLabel(1, "Value");

  m_datastore = datastore;

  loadTable();
}

void AssemblyStats::addRow(QString tag, QString value)
{
  //cerr << "Adding " << m_currow << ":" << tag << " " << value << endl;
  m_table->setItem(m_currow, 0, new QTableItem(m_table, QTableItem::Never, tag));
  m_table->setItem(m_currow, 1, new QTableItem(m_table, QTableItem::Never, value));

  m_currow++;
}

void AssemblyStats::addRow(QString tag, int value)
{
  addRow(tag, QString::number(value, 'g', 12));
}

void AssemblyStats::addRow(QString tag, double  value)
{
  addRow(tag, QString::number(value, 'g', 12));
}


void AssemblyStats::loadTable()
{
  QCursor orig = cursor();
  m_currow = 0;

  if (m_datastore->scaffold_bank)
  {
    addRow("[Scaffolds]", "");
    addRow("TotalScaffolds", m_datastore->scaffold_bank.getSize());

    addRow("", "");
  }

  int LARGETHRESHOLD = 10000;

  if (m_datastore->contig_bank)
  {
    m_datastore->contig_bank.seekg(1);
    AMOS::Contig_t ctg;
    vector<int> sizes;
    vector<int> largesizes;
    vector<int> smallsizes;
    int totalsize = 0;
    int totalreads = 0;

    int largecount = 0;
    int largesize = 0;
    int largereads = 0;

    int smallcount = 0;
    int smallsize = 0;
    int smallreads = 0;


    while (m_datastore->contig_bank >> ctg)
    {
      int l = ctg.getLength();
      int r = ctg.getReadTiling().size();

      sizes.push_back(l);
      totalsize += l;
      totalreads += r;

      if (l >= LARGETHRESHOLD)
      {
        largecount++;
        largesize += l;
        largereads += r;
      }
      else
      {
        smallcount++;
        smallsize += l;
        smallreads += r;
      }
    }

    sort(sizes.begin( ), sizes.end( ), greater<int>( ) );

    int thresh = totalsize / 2;
    int cum = 0;
    int n50 = 0;

    int maxlarge = 0;
    int minlarge = 0;
    int maxsmall = 0;
    int minsmall = 0;

    for (int i = sizes.size()-1; i >= 0; i--)
    {
      int cur = sizes[i];
      cum += cur;

      if (!n50 && cum >= thresh) { n50 = i; }

      if (cur >= LARGETHRESHOLD)
      {
        if (maxlarge == 0){maxlarge = cur;}
        minlarge = cur;
      }
      else
      {
        if (maxsmall == 0){maxsmall = cur;}
        minsmall = cur;
      }
    }


    // Contigs
    addRow("[Contigs]", "");
    addRow("TotalContigs", m_datastore->contig_bank.getSize());
    addRow("TotalBasesInContigs", totalsize);
    addRow("MeanContigSize", DIV(totalsize, sizes.size()));
    addRow("MinContigSize", sizes[0]);
    addRow("MaxContigSize", sizes[sizes.size()-1]);
    addRow("N50ContigBases", sizes[n50]);
    addRow("", "");

    // Big Contigs
    addRow(QString("[BigContigs_greater_") + QString::number(LARGETHRESHOLD) + "]", "");
    addRow("TotalBigContigs", largecount);
    addRow("BigContigLength", largesize);
    addRow("MeanBigContigSize", DIV(largesize,largecount));
    addRow("MinBigContig", minlarge);
    addRow("MaxBigContig", maxlarge);
    addRow("BigContigsPercentBases", DIV(largesize,totalsize)*100.0);
    addRow("", "");

    // Small Contigs
    addRow("TotalSmallContigs", smallcount);
    addRow("SmallContigLength", smallsize);
    addRow("MeanSmallContigSize", DIV(smallsize, smallcount));
    addRow("MinSmallContig", minsmall);
    addRow("MaxSmallContig", maxsmall);
    addRow("SmallContigsPercentBases", DIV(smallsize,totalsize)*100.0);
    addRow("", "");



    // Reads
    addRow("[Reads]", "");
    addRow("TotalReads", m_datastore->read_bank.getSize());
    addRow("ReadsInContigs", totalreads);
    addRow("BigContigReads", largereads);
    addRow("SmallContigReads", smallreads);
    addRow("SingletonReads", m_datastore->read_bank.getSize() - totalreads);
  }

  m_table->adjustColumn(0);
  m_table->setColumnWidth(1, 250);
}

void AssemblyStats::refreshTable()
{
  loadTable();
}
