#include "ContigPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include "foundation_AMOS.hh"

#include <vector>

using namespace std;



class ContigListItem : public QListViewItem
{
public:
  ContigListItem(QListView * parent, 
                 QString id,
                 QString iid,
                 QString eid,
                 QString length,
                 QString reads)
               
    : QListViewItem(parent, id, iid, eid, length, reads) {}

  int compare(QListViewItem *i, int col,
              bool ascending ) const
  {
    return atoi(key(col,ascending)) - atoi(i->key(col,ascending));
  }
};


ContigPicker::ContigPicker(const string & bankname,
                           QWidget * parent, 
                           const char * name)
  :QMainWindow(parent, name)
{
  QListView * table = new QListView(this, "contigpickertbl");
  table->resize(500,500);
  setCentralWidget(table);
  resize(300,300);
  show();

  connect(table, SIGNAL(doubleClicked(QListViewItem *)),
          this,  SLOT(itemSelected(QListViewItem *)));

  table->addColumn("Id");
  table->addColumn("IID");
  table->addColumn("EID");
  table->addColumn("Length");
  table->addColumn("Reads");
  table->setShowSortIndicator(true);
  table->setRootIsDecorated(true);

  try
  {
    QCursor orig = cursor();
    setCursor(Qt::waitCursor);
    AMOS::Contig_t contig;
    AMOS::BankStream_t contig_bank(AMOS::Contig_t::NCODE);

    contig_bank.open(bankname, AMOS::B_SPY);

    QString status = "Select from " + 
                     QString::number(contig_bank.getSize()) + 
                     " contigs in " + bankname.c_str();
    statusBar()->message(status);

    int contigid = 1;
    while (contig_bank >> contig)
    {
      int contiglen = contig.getSeqString().length();
      int numreads = contig.getReadTiling().size();

      //ContigListItem * contigitem = 
      new ContigListItem(table,  
                         QString::number(contigid), 
                         QString::number(contig.getIID()),
                         contig.getEID(),
                         QString::number(contiglen), 
                         QString::number(numreads));
      contigid++;
    }
    setCursor(orig);

  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: -- Fatal AMOS Exception --\n" << e;
  }
}

void ContigPicker::itemSelected(QListViewItem * item)
{
  emit contigSelected(atoi(item->text(0)));
}

