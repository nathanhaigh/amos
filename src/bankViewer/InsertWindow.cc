#include "InsertWindow.hh"
#include "InsertWidget.hh"

#include <qstatusbar.h>
#include <qlabel.h>
#include <qslider.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>

using namespace std;

InsertWindow::InsertWindow(DataStore * datastore,
                           QWidget * parent,
                           const char * name)
  : QMainWindow(parent, name)
{
  resize(600, 300);

  QToolBar * options = new QToolBar(this, "options");
  options->setLabel("Options");

  new QLabel("Zoom", options, "zoomlbl");
  QSlider * zoom = new QSlider(1, 500, 10, 20, Qt::Horizontal, options, "zoom");


  QToolButton * bShowTypes = new QToolButton(QPixmap(), "Show Types", "Show Types", 
                                             this, SLOT(showInserts()), options);
  bShowTypes->setText("Display Types");
  bShowTypes->setPopupDelay(1);

  m_display = new QPopupMenu(this);
  bShowTypes->setPopup(m_display);

  m_types['H'].first = m_display->insertItem("Happy",        this, SLOT(toggleHappy()));
  m_types['S'].first = m_display->insertItem("Stretched",    this, SLOT(toggleStretched()));
  m_types['O'].first = m_display->insertItem("Orientation",  this, SLOT(toggleOrientation()));
  m_types['M'].first = m_display->insertItem("Missing Mate", this, SLOT(toggleMissing()));
  m_types['L'].first = m_display->insertItem("Linking Mate", this, SLOT(toggleLinking()));
  m_types['N'].first = m_display->insertItem("No Mate",      this, SLOT(toggleNone()));
  m_types['U'].first = m_display->insertItem("Unknown",      this, SLOT(toggleUnknown()));

  map<char, pair<int, bool> >::iterator mi;

  for (mi =  m_types.begin();
       mi != m_types.end();
       mi++)
  {
    mi->second.second = true;
    m_display->setItemChecked(mi->second.first, TRUE);
  }

  InsertWidget * iw = new InsertWidget(datastore, m_types, this, "iw");
  setCentralWidget(iw);
  statusBar()->message("Ready", 2000);
  statusBar()->setSizeGripEnabled(false);

  connect(iw,          SIGNAL(setStatus(const QString &)),
          statusBar(), SLOT(message(const QString &)));

  connect(iw,   SIGNAL(setGindex(int)),
          this, SIGNAL(setGindex(int)));

  connect(this, SIGNAL(setTilingVisibleRange(int, int)),
          iw,   SLOT(setTilingVisibleRange(int, int)));

  connect(zoom, SIGNAL(valueChanged(int)),
          iw,   SLOT(setZoom(int)));

  connect(this, SIGNAL(refreshCanvas()),
          iw,   SLOT(refreshCanvas()));
}

void InsertWindow::contigChanged()
{
  emit refreshCanvas();
}

void InsertWindow::toggleHappy()       { toggleItem('H'); }
void InsertWindow::toggleUnknown()     { toggleItem('U'); }
void InsertWindow::toggleStretched()   { toggleItem('S'); }
void InsertWindow::toggleOrientation() { toggleItem('O'); }
void InsertWindow::toggleMissing()     { toggleItem('M'); }
void InsertWindow::toggleLinking()     { toggleItem('L'); }
void InsertWindow::toggleNone()        { toggleItem('N'); }

void InsertWindow::toggleItem(char c)
{
  m_types[c].second = !m_display->isItemChecked(m_types[c].first);
  m_display->setItemChecked(m_types[c].first, m_types[c].second);

  emit refreshCanvas();
}
