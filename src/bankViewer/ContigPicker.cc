#include "ContigPicker.hh"
#include <qcursor.h>
#include <qstatusbar.h>

#include "foundation_AMOS.hh"



class ContigListItem : public QListViewItem
{
public:
  ContigListItem(QListView * parent, 
                 QString id,
                 QString type,
                 QString offset,
                 QString length,
                 QString elements)
               
    : QListViewItem(parent, id, type, offset, length, elements) {}

  ContigListItem(QListViewItem * parent, 
                 QString id,
                 QString type,
                 QString offset,
                 QString length,
                 QString elements)
               
    : QListViewItem(parent, id, type, offset, length, elements) {}

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
  table->addColumn("Type");
  table->addColumn("Offset");
  table->addColumn("Length");
  table->addColumn("Elements");
  table->setShowSortIndicator(true);
  table->setRootIsDecorated(true);

  try
  {
    QCursor orig = cursor();
    setCursor(Qt::waitCursor);
    AMOS::Contig_t contig;
    AMOS::BankStream_t contig_bank(AMOS::Contig_t::NCODE);

    contig_bank.open(bankname);

    QString status = "Select from " + 
                     QString::number(contig_bank.getSize()) + 
                     " contigs in " + bankname.c_str();
    statusBar()->message(status);

    vector<ContigListItem> contigs;

    int contigid = 1;
    while (contig_bank >> contig)
    {
      int contiglen = contig.getSeqString().length();
      int numreads = contig.getReadTiling().size();

      //ContigListItem * contigitem = 
      new ContigListItem(table,  
                         QString::number(contigid), 
                         "Contig",
                         "0",
                         QString::number(contiglen), 
                         QString::number(numreads));

      /*
      vector<AMOS::Tile_t>::iterator ti;
      for (ti =  contig.getReadTiling().begin();
           ti != contig.getReadTiling().end();
           ti++)
      {
        new ContigListItem(contigitem,
                           QString::number(ti->source),
                           "Read",
                           QString::number(ti->offset),
                           QString::number(ti->range.getLength() + ti->gaps.size()),
                           " ");
      }
      */
      
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

