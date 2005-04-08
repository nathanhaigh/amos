#include "InsertWindow.hh"
#include "InsertWidget.hh"

#include <qstatusbar.h>
#include <qlabel.h>
#include <qslider.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qaccel.h>
#include <qmenubar.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qlineedit.h>

#include "UIElements.hh"


using namespace std;

InsertWindow::InsertWindow(DataStore * datastore,
                           QWidget * parent,
                           const char * name)
  : QMainWindow(parent, name)
{
  resize(600, 300);
  setCaption("Inserts");

  statusBar()->message("Ready", 2000);
  statusBar()->setSizeGripEnabled(false);

  // Toolbar
  QToolBar * options = new QToolBar(this, "options");
  options->setLabel("Options");

  new QLabel("Zoom", options, "zoomlbl");
  QSlider * zoom = new QSlider(1, 500, 10, 20, Qt::Horizontal, options, "zoom");

  new QLabel("IID:", options, "iidlbl");
  QLineEdit * iidpick = new QLineEdit(options, "iidpick");

  new QLabel("EID:", options, "eidlbl");
  QLineEdit * eidpick = new QLineEdit(options, "eidpick");


  // MenuBar
  m_typesmenu = new QPopupMenu(this);
  menuBar()->insertItem("&Display Types", m_typesmenu);

  const char * states = Insert::allstates;
  for (unsigned int i = 0; i < strlen(states); i++)
  {
    char state = states[i];
    QPixmap rect(10,10);

    QPainter p(&rect);
    p.fillRect(rect.rect(), UIElements::getInsertColor((Insert::MateState)state));
    p.end();

    QString name = Insert::getInsertTypeStr((Insert::MateState)state);

    m_types[state].first = m_typesmenu->insertItem(QIconSet(rect), name);
    m_types[state].second = true;
    m_typesmenu->setItemChecked(m_types[state].first, m_types[state].second);
  }

  m_optionsmenu = new QPopupMenu(this);
  menuBar()->insertItem("&Options", m_optionsmenu);
  m_scaffid = m_optionsmenu->insertItem("&Scaffold Plot", this, SLOT(togglePaintScaffold()));

  m_connectmatesid = m_optionsmenu->insertItem("&Connect Mates", this, SLOT(toggleConnectMates()));
  m_optionsmenu->setItemChecked(m_connectmatesid, true);

  m_partitiontypesid = m_optionsmenu->insertItem("&Partition Types", this, SLOT(togglePartitionTypes()));
  m_optionsmenu->setItemChecked(m_partitiontypesid, true);

  m_coverageid = m_optionsmenu->insertItem("Coverage Plo&t", this, SLOT(toggleCoveragePlot()));
  m_optionsmenu->setItemChecked(m_coverageid, true);

  m_featid = m_optionsmenu->insertItem("Show F&eatures", this, SLOT(toggleFeatures()));
  m_optionsmenu->setItemChecked(m_featid, true);

  m_libcolorid = m_optionsmenu->insertItem("Color By &Library", this, SLOT(toggleColorByLibrary()));
  m_optionsmenu->setItemChecked(m_libcolorid, false);

  // Main Widget
  InsertWidget * iw = new InsertWidget(datastore, m_types, this, "iw");
  setCentralWidget(iw);

  connect(m_typesmenu, SIGNAL(activated(int)),
          this,        SLOT(toggleItem(int)));

  connect(iw,          SIGNAL(setStatus(const QString &)),
          statusBar(), SLOT(message(const QString &)));

  connect(iw,   SIGNAL(setGindex(int)),
          this, SIGNAL(setGindex(int)));

  connect(iw,   SIGNAL(setContigId(int)),
          this, SIGNAL(setContigId(int)));

  connect(this, SIGNAL(setTilingVisibleRange(int, int)),
          iw,   SLOT(setTilingVisibleRange(int, int)));

  connect(zoom, SIGNAL(valueChanged(int)),
          iw,   SLOT(setZoom(int)));

  connect(this, SIGNAL(paintCanvas()),
          iw,   SLOT(paintCanvas()));

  connect(iidpick, SIGNAL(textChanged(const QString &)),
          iw,      SIGNAL(highlightIID(const QString &)));

  connect(eidpick, SIGNAL(textChanged(const QString &)),
          iw,      SIGNAL(highlightEID(const QString &)));

  connect(this, SIGNAL(setConnectMates(bool)),
          iw,   SLOT(setConnectMates(bool)));

  connect(this, SIGNAL(setPartitionTypes(bool)),
          iw,   SLOT(setPartitionTypes(bool)));

  connect(this, SIGNAL(setCoveragePlot(bool)),
          iw,   SLOT(setCoveragePlot(bool)));

  connect(this, SIGNAL(setPaintScaffold(bool)),
          iw,   SLOT(setPaintScaffold(bool)));

  connect(this, SIGNAL(setFeatures(bool)),
          iw,   SLOT(setFeatures(bool)));

  connect(this, SIGNAL(setColorByLibrary(bool)),
          iw,   SLOT(setColorByLibrary(bool)));

  connect(this, SIGNAL(newContig()),
          iw,   SLOT(contigChanged()));

  QAccel *a = new QAccel( this );
  a->connectItem(a->insertItem(CTRL+SHIFT+Key_S), iw, SLOT(start()) );
  a->connectItem(a->insertItem(Key_Left), iw, SLOT(left()) );
  a->connectItem(a->insertItem(Key_Right), iw, SLOT(right()) );
  a->connectItem(a->insertItem(Key_Escape), iw, SLOT(stopbreak()));
  a->connectItem(a->insertItem(CTRL+SHIFT+Key_A), iw, SLOT(autoplay()) );
}

void InsertWindow::contigChanged()
{
  emit newContig();
}

void InsertWindow::toggleItem(int id)
{
  typemap::iterator mi;

  for (mi = m_types.begin(); mi != m_types.end(); mi++)
  {
    if (mi->second.first == id)
    {
      mi->second.second = !m_typesmenu->isItemChecked(mi->second.first);
      m_typesmenu->setItemChecked(mi->second.first, mi->second.second);

      emit paintCanvas();
      break;
    }
  }
}

void InsertWindow::toggleConnectMates()
{
  bool b = !m_optionsmenu->isItemChecked(m_connectmatesid);
  m_optionsmenu->setItemChecked(m_connectmatesid, b);

  emit setConnectMates(b);
}

void InsertWindow::togglePartitionTypes()
{
  bool b = !m_optionsmenu->isItemChecked(m_partitiontypesid);
  m_optionsmenu->setItemChecked(m_partitiontypesid, b);

  emit setPartitionTypes(b);
}

void InsertWindow::toggleCoveragePlot()
{
  bool b = !m_optionsmenu->isItemChecked(m_coverageid);
  m_optionsmenu->setItemChecked(m_coverageid, b);

  emit setCoveragePlot(b);
}

void InsertWindow::togglePaintScaffold()
{
  bool b = !m_optionsmenu->isItemChecked(m_scaffid);
  m_optionsmenu->setItemChecked(m_scaffid, b);

  emit setPaintScaffold(b);
}

void InsertWindow::toggleFeatures()
{
  bool b = !m_optionsmenu->isItemChecked(m_featid);
  m_optionsmenu->setItemChecked(m_featid, b);

  emit setFeatures(b);
}

void InsertWindow::toggleColorByLibrary()
{
  bool b = !m_optionsmenu->isItemChecked(m_libcolorid);
  m_optionsmenu->setItemChecked(m_libcolorid, b);

  emit setColorByLibrary(b);
}
