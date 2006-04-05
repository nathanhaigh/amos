#include "AssemblyStats.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include <qlabel.h>
#include <qlineedit.h>

#include "foundation_AMOS.hh"
#include "DataStore.hh"


#include <vector>

using namespace std;
using namespace AMOS;


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
  m_table->horizontalHeader()->setLabel(0, "Field");
  m_table->horizontalHeader()->setLabel(1, "Value");

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

void AssemblyStats::addRow(QString tag, double value)
{
  addRow(tag, QString::number(value, 'f', 2));
}


void AssemblyStats::loadTable()
{
  QCursor orig = cursor();
  m_currow = 0;

  if (m_datastore->scaffold_bank.isOpen())
  {
    vector<int> bases;
    vector<int> spans;
    vector<int> contigs;
    int totalcontigs = 0;
    int totalbases = 0;
    int totalspan = 0;
    int numscaffolds = 0;

    int largescaffolds = 0;
    int largescaffoldspan = 0;
    int largescaffoldThreshold = 2000;

    AMOS::Scaffold_t scf;

    m_datastore->scaffold_bank.seekg(1);
    while (m_datastore->scaffold_bank >> scf)
    {
      int s = scf.getSpan();
      int b = 0;
      vector<Tile_t>::const_iterator ci;
      const vector<Tile_t> & ctiling = scf.getContigTiling();
      for (ci =  ctiling.begin();
           ci != ctiling.end();
           ci++)
      {
        b+=ci->getGappedLength();
      }

      totalcontigs += ctiling.size();
      totalbases += b;
      totalspan += s;

      contigs.push_back(ctiling.size());
      spans.push_back(s);
      bases.push_back(b);

      numscaffolds++;

      if (s >= largescaffoldThreshold)
      {
        largescaffolds++;
        largescaffoldspan += s;
      }
    }

    sort(contigs.begin(), contigs.end(), less<int>());
    sort(bases.begin(),   bases.end(),   less<int>());
    sort(spans.begin(),   spans.end(),   less<int>());

    int n50bases = 0;

    int basesthreshold = totalbases/2;
    int curbases = 0;

    for (n50bases = numscaffolds-1; n50bases >= 0; n50bases--)
    {
      curbases += bases[n50bases];
      if (curbases >= basesthreshold)
      {
        break;
      }
    }






    addRow("[Scaffolds]", "");
    addRow("TotalScaffolds", numscaffolds);
    addRow("TotalContigsInScaffolds", totalcontigs);
    addRow("MeanContigsPerScaffold", DIV(totalcontigs, numscaffolds));
    addRow("MinContigsPerScaffold", contigs[0]);
    addRow("MaxContigsPerScaffold", contigs[contigs.size()-1]);
    addRow("", "");

    addRow("TotalBasesInScaffolds", totalbases);
    addRow("MeanBasesInScaffolds", DIV(totalbases, numscaffolds));
    addRow("MaxBasesInScaffolds", bases[numscaffolds-1]);
    addRow("N50ScaffoldBases", bases[n50bases]);
    addRow("", "");


    addRow("TotalSpanOfScaffolds", totalspan);
    addRow("MeanSpanOfScaffolds", DIV(totalspan, numscaffolds));
    addRow("MinScaffoldSpan", spans[0]);
    addRow("MaxScaffoldSpan", spans[numscaffolds-1]);
    addRow("IntraScaffoldGaps", totalcontigs - numscaffolds);
    addRow("2KbScaffolds", largescaffolds);
    addRow("2KbScaffoldSpan", largescaffoldspan);
    addRow("2KbScaffoldPercent", DIV(largescaffoldspan, totalspan) * 100.0);
    addRow("MeanSequenceGapSize", DIV(totalspan-totalbases, totalcontigs-numscaffolds));

    addRow("", "");
  }

  int LARGETHRESHOLD = 10000;

  if (m_datastore->contig_bank.isOpen())
  {
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


    m_datastore->contig_bank.seekg(1);
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

    sort(sizes.begin( ), sizes.end( ), less<int>( ) );

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
    addRow("TotalContigs",  (int)m_datastore->contig_bank.getSize());
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
    addRow("[SmallContigs]", "");
    addRow("TotalSmallContigs", smallcount);
    addRow("SmallContigLength", smallsize);
    addRow("MeanSmallContigSize", DIV(smallsize, smallcount));
    addRow("MinSmallContig", minsmall);
    addRow("MaxSmallContig", maxsmall);
    addRow("SmallContigsPercentBases", DIV(smallsize,totalsize)*100.0);
    addRow("", "");



    // Reads
    addRow("[Reads]", "");
    addRow("TotalReads", (int)m_datastore->read_bank.getSize());
    addRow("ReadsInContigs", totalreads);
    addRow("BigContigReads", largereads);
    addRow("SmallContigReads", smallreads);
    addRow("SingletonReads", (int) m_datastore->read_bank.getSize() - totalreads);
  }

  m_table->adjustColumn(0);
  m_table->setColumnWidth(1, 250);
}

void AssemblyStats::refreshTable()
{
  loadTable();
}
