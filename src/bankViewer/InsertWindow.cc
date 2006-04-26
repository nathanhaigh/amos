#include "InsertWindow.hh"
#include "InsertWidget.hh"

#include <qstatusbar.h>
#include <qlabel.h>
#include <qslider.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qaccel.h>
#include <qmenubar.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qlineedit.h>
#include <qdockarea.h>
#include <qtextedit.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qradiobutton.h>

#include "DataStore.hh"
#include "UIElements.hh"
#include "BufferedLineEdit.hh"
#include "QueryWidget.hh"
#include "DetailWidget.hh"

#include "icons/zoom_in.xpm"
#include "icons/zoom_out.xpm"
#include "icons/pointer_tool.xpm"


using namespace std;

InsertWindow::InsertWindow(DataStore * datastore,
                           QWidget * parent,
                           const char * name)
  : QMainWindow(parent, name),
    m_datastore(datastore)
{
  resize(800,600);
  setCaption("AI - Inserts");

  const char * states = Insert::allstates;
  unsigned int type = 0;

  // Libraries
  m_libmenu = new QPopupMenu(this);
  menuBar()->insertItem("&Libraries", m_libmenu);
  buildLibraryMenu();


  QPopupMenu * m_featmenu = new QPopupMenu(this);
  menuBar()->insertItem("&Features", m_featmenu);

  for (type = 0; type < strlen(UIElements::allFeatureTypes); type++)
  {
    char feat = UIElements::allFeatureTypes[type];
    QPixmap rect(10,10);

    QPainter p(&rect);
    p.fillRect(rect.rect(), UIElements::getFeatureColor((AMOS::FeatureType_t)feat));
    p.end();

    QString name = (QString)"[" + feat + "] ";
    name += UIElements::getFeatureStr((AMOS::FeatureType_t)feat);

    m_featmenu->insertItem(QIconSet(rect), name);
  }

  // Main Widget
  m_inserts = new InsertWidget(datastore, m_types, this, "iw");
  InsertWidget * iw = m_inserts;
  setCentralWidget(iw);



  QToolBar * tools = new QToolBar(this, "tools");
  QIconSet icon_pointer(QPixmap((const char ** )pointer_tool));
  QIconSet icon_zoomin(QPixmap((const char ** )zoom_in));
  QIconSet icon_zoomout(QPixmap((const char **)zoom_out));

  QToolButton * b = new QToolButton(tools, "select");
  b->setIconSet(icon_pointer);
  b->setTextLabel("select tool");
  b->setMaximumWidth(25);
  connect(b, SIGNAL(clicked()), iw, SIGNAL(setSelectTool()));

  b = new QToolButton(tools, "zoomin");
  b->setIconSet(icon_zoomin);
  b->setTextLabel("zoom in");
  b->setMaximumWidth(25);
  connect(b, SIGNAL(clicked()), iw, SIGNAL(setZoomInTool()));

  b = new QToolButton(tools, "zoomout");
  b->setIconSet(icon_zoomout);
  b->setTextLabel("zoom out");
  b->setMaximumWidth(25);
  connect(b, SIGNAL(clicked()), iw, SIGNAL(setZoomOutTool()));


  // Query Dock
  QDockWindow * queryDock = new QDockWindow (QDockWindow::InDock, this);
  QScrollView * queryView = new QScrollView (queryDock, "queryview");
  queryView->setResizePolicy (QScrollView::AutoOneFit);
  m_query = new QueryWidget (queryView, "queries");
  queryView->addChild (m_query);
  queryDock->setWidget (queryView);
  queryDock->setResizeEnabled (true);
  addDockWindow (queryDock, Qt::DockRight);


  // Happy distance
  m_query->happyEdit->setText (QString::number(Insert::MAXSTDEV));


  // Feature filters
  QPalette p (m_query->palette());

  p.setColor (QColorGroup::Button, UIElements::color_featinsertcov);
  m_query->insertSlider->setPalette(p);

  p.setColor (QColorGroup::Button, UIElements::color_featreadcov);
  m_query->readSlider->setPalette(p);

  p.setColor (QColorGroup::Button, UIElements::color_featsnp);
  m_query->snpSlider->setPalette(p);

  p.setColor (QColorGroup::Button, UIElements::color_featunitig);
  m_query->unitigSlider->setPalette(p);

  p.setColor (QColorGroup::Button, UIElements::color_featcoverage);
  m_query->qcSlider->setPalette(p);

  p.setColor (QColorGroup::Button, UIElements::color_featbreak);
  m_query->breakSlider->setPalette(p);


  // Mate types
  m_query->happyLabel->setPixmap
    (mateIcon(UIElements::color_Happy, 0));
  m_types[Insert::Happy].first = 7;
  m_types[Insert::Happy].second =
    m_query->happyCheck->isChecked();

  m_query->stretchedLabel->setPixmap
    (mateIcon(UIElements::color_StretchedMate,0));
  m_types[Insert::StretchedMate].first = 6;
  m_types[Insert::StretchedMate].second =
    m_query->stretchedCheck->isChecked();

  m_query->compressedLabel->setPixmap
    (mateIcon(UIElements::color_CompressedMate,0));
  m_types[Insert::CompressedMate].first = 5;
  m_types[Insert::CompressedMate].second =
    m_query->compressedCheck->isChecked();

  m_query->orientationLabel->setPixmap
    (mateIcon(UIElements::color_OrientationViolation,1));
  m_types[Insert::OrientationViolation].first = 4;
  m_types[Insert::OrientationViolation].second =
    m_query->orientationCheck->isChecked();

  m_query->linkingLabel->setPixmap
    (mateIcon(UIElements::color_MissingMate,1));
  m_types[Insert::MissingMate].first = 3;
  m_types[Insert::MissingMate].second =
    m_query->linkingCheck->isChecked();

  m_query->singletonLabel->setPixmap
    (mateIcon(UIElements::color_SingletonMate,1));
  m_types[Insert::SingletonMate].first = 2;
  m_types[Insert::SingletonMate].second =
    m_query->singletonCheck->isChecked();

  m_query->unmatedLabel->setPixmap
    (mateIcon(UIElements::color_NoMate,2));
  m_types[Insert::NoMate].first = 1;
  m_types[Insert::NoMate].second =
    m_query->unmatedCheck->isChecked();


  // Display
  iw->setCoveragePlot(m_query->coverageCheck->isChecked());
  iw->setCEStatistic(m_query->ceCheck->isChecked());
  iw->setConnectMates(m_query->mateCheck->isChecked());
  iw->setPartitionTypes(m_query->partitionCheck->isChecked());
  iw->setTintHappiness(m_query->tintCheck->isChecked());


  // Mate Colors
  iw->setColorByStretchiness(m_query->continuousButton->isChecked());
  iw->setColorByMate(m_query->linkingButton->isChecked());
  iw->setColorByLibrary(m_query->libraryButton->isChecked());


  // Detail Dock
  QDockWindow * detailDock = new QDockWindow (QDockWindow::InDock, this);
  m_detail = new DetailWidget (detailDock, "details");
  detailDock->setWidget (m_detail);
  detailDock->setResizeEnabled (true);
  addDockWindow (detailDock, Qt::DockRight);


  // searching and happy distance
  connect(m_query->searchButton, SIGNAL(clicked()),
          this,                  SLOT(loadSearch()));

  connect(this,                  SIGNAL(search(const QString &)),
          iw,                    SIGNAL(search(const QString &)));

  connect(m_query->happyButton,  SIGNAL(clicked()),
          this,                  SLOT(loadHappyDistance()));

  connect(this,                  SIGNAL(setHappyDistance(float)),
          iw,                    SLOT(setHappyDistance(float)));

  // feature filters
  connect(iw,                    SIGNAL(newMaxInsertCovTol(int)),
          this,                  SLOT(setMaxInsertCovTol(int)));

  connect(m_query->insertCheck,  SIGNAL(toggled(bool)),
          iw,                    SLOT(setInsertCovFeatures(bool)));

  connect(m_query->insertSlider, SIGNAL(valueChanged(int)),
          iw,                    SLOT(setInsertCovTol(int)));

  connect(iw,                    SIGNAL(newMaxReadCovTol(int)),
          this,                  SLOT(setMaxReadCovTol(int)));

  connect(m_query->readSlider,   SIGNAL(valueChanged(int)),
          iw,                    SLOT(setReadCovTol(int)));

  connect(m_query->readCheck,    SIGNAL(toggled(bool)),
          iw,                    SLOT(setReadCovFeatures(bool)));

  connect(iw,                    SIGNAL(newMaxSNPTol(int)),
          this,                  SLOT(setMaxSNPTol(int)));

  connect(m_query->snpSlider,    SIGNAL(valueChanged(int)),
          iw,                    SLOT(setSNPTol(int)));

  connect(m_query->snpCheck,     SIGNAL(toggled(bool)),
          iw,                    SLOT(setSNPFeatures(bool)));

  connect(iw,                    SIGNAL(newMaxUnitigTol(int)),
          this,                  SLOT(setMaxUnitigTol(int)));

  connect(m_query->unitigSlider, SIGNAL(valueChanged(int)),
          iw,                    SLOT(setUnitigTol(int)));

  connect(m_query->unitigCheck,  SIGNAL(toggled(bool)),
          iw,                    SLOT(setUnitigFeatures(bool)));

  connect(iw,                    SIGNAL(newMaxQCTol(int)),
          this,                  SLOT(setMaxQCTol(int)));

  connect(m_query->qcSlider,     SIGNAL(valueChanged(int)),
          iw,                    SLOT(setQCTol(int)));

  connect(m_query->qcCheck,      SIGNAL(toggled(bool)),
          iw,                    SLOT(setQCFeatures(bool)));

  connect(iw,                    SIGNAL(newMaxBreakTol(int)),
          this,                  SLOT(setMaxBreakTol(int)));

  connect(m_query->breakSlider,  SIGNAL(valueChanged(int)),
          iw,                    SLOT(setBreakTol(int)));

  connect(m_query->breakCheck,   SIGNAL(toggled(bool)),
          iw,                    SLOT(setBreakFeatures(bool)));

  connect(m_query->otherCheck,   SIGNAL(toggled(bool)),
          iw,                    SLOT(setOtherFeatures(bool)));

  // mate type toggles
  connect(m_query->happyCheck,       SIGNAL(toggled(bool)),
          this,                      SLOT(toggleItem()));

  connect(m_query->stretchedCheck,   SIGNAL(toggled(bool)),
          this,                      SLOT(toggleItem()));

  connect(m_query->compressedCheck,  SIGNAL(toggled(bool)),
          this,                      SLOT(toggleItem()));

  connect(m_query->linkingCheck,     SIGNAL(toggled(bool)),
          this,                      SLOT(toggleItem()));

  connect(m_query->orientationCheck, SIGNAL(toggled(bool)),
          this,                      SLOT(toggleItem()));

  connect(m_query->singletonCheck,   SIGNAL(toggled(bool)),
          this,                      SLOT(toggleItem()));

  connect(m_query->unmatedCheck,     SIGNAL(toggled(bool)),
          this,                      SLOT(toggleItem()));

  // display toggles
  connect(m_query->coverageCheck,    SIGNAL(toggled(bool)),
          iw,                        SLOT(setCoveragePlot(bool)));

  connect(m_query->ceCheck,          SIGNAL(toggled(bool)),
          iw,                        SLOT(setCEStatistic(bool)));

  connect(m_query->mateCheck,        SIGNAL(toggled(bool)),
          iw,                        SLOT(setConnectMates(bool)));

  connect(m_query->partitionCheck,   SIGNAL(toggled(bool)),
          iw,                        SLOT(setPartitionTypes(bool)));

  connect(m_query->tintCheck,        SIGNAL(toggled(bool)),
          iw,                        SLOT(setTintHappiness(bool)));

  // mate coloring
  connect(m_query->continuousButton, SIGNAL(toggled(bool)),
          iw,                        SLOT(setColorByStretchiness(bool)));

  connect(m_query->linkingButton,    SIGNAL(toggled(bool)),
          iw,                        SLOT(setColorByMate(bool)));

  connect(m_query->libraryButton,    SIGNAL(toggled(bool)),
          iw,                        SLOT(setColorByLibrary(bool)));

  // detail text
  connect(iw,   SIGNAL(setDetails(const QString &)),
          m_detail->detailText, SLOT(setText(const QString &)));

  // canvas operations
  connect(iw,   SIGNAL(setGindex(int)),
          this, SIGNAL(setGindex(int)));

  connect(iw,   SIGNAL(setContigId(int)),
          this, SIGNAL(setContigId(int)));

  connect(this, SIGNAL(setTilingVisibleRange(int, int, int)),
          iw,   SLOT(setTilingVisibleRange(int, int, int)));

  connect(this, SIGNAL(paintCanvas()),
          iw,   SLOT(paintCanvas()));

  connect(this, SIGNAL(newContig()),
          iw,   SLOT(contigChanged()));

  connect(this, SIGNAL(highlightRead(int)),
          iw,   SIGNAL(highlightRead(int)));

  connect(parent, SIGNAL(bankSelected()),
          iw,   SLOT(refreshWidget()));

  connect(iw, SIGNAL(jumpToRead(int)),
          parent, SLOT(jumpToRead(int)));


  if (s_persistant)
  {
    // Persistant Toolbar
    QToolBar * persistant = new QToolBar(this, "persistant");
    persistant->setLabel("Persistant");

    new QLabel("Persistant", persistant, "persistlbl");
    QCheckBox * persist = new QCheckBox(persistant, "persistantcheck");

    new QLabel("Error rate", persistant, "eratelbl");
    QSpinBox * spin = new QSpinBox(0,19,1,persistant, "eratespin");

    connect(persist, SIGNAL(toggled(bool)),
            iw,      SLOT(setPersistant(bool)));
    connect(spin,    SIGNAL(valueChanged(int)),
            iw,      SLOT(setErrorRate(int)));
  }

  iw->paintCanvas(); // TODO: Is this necessary?
}

int InsertWindow::s_persistant(0);

void InsertWindow::buildLibraryMenu()
{
  m_libmenu->clear();

  unsigned int type = 0;
  const char * states = Insert::allstates;

  DataStore::LibLookup_t::iterator li;
  for (li =  m_datastore->m_libdistributionlookup.begin();
       li != m_datastore->m_libdistributionlookup.end();
       li++)
  {
    char state = states[type];
    QPixmap rect(10,10);

    QPainter p(&rect);
    p.fillRect(rect.rect(), UIElements::getInsertColor((Insert::MateState)state));
    p.end();

    QString name = QString::number(li->first); 
    name += " [" + QString::number(li->second.mean); 
    name += " +/- " + QString::number(li->second.sd);
    name += "]";

    m_libmenu->insertItem(QIconSet(rect), name);

    type++;
    if (type >= strlen(Insert::allstates)) { type = 0; }
  }
}


void InsertWindow::loadHappyDistance()
{
  emit setHappyDistance(m_query->happyEdit->text().toFloat());
}

void InsertWindow::loadSearch()
{
  emit search(m_query->searchEdit->text());
}

void InsertWindow::contigChanged()
{
  emit newContig();
}

void InsertWindow::toggleItem()
{
  m_types[Insert::Happy].second =
    m_query->happyCheck->isChecked();
  m_types[Insert::StretchedMate].second =
    m_query->stretchedCheck->isChecked();
  m_types[Insert::CompressedMate].second =
    m_query->compressedCheck->isChecked();
  m_types[Insert::MissingMate].second =
    m_query->linkingCheck->isChecked();
  m_types[Insert::OrientationViolation].second =
    m_query->orientationCheck->isChecked();
  m_types[Insert::SingletonMate].second =
    m_query->singletonCheck->isChecked();
  m_types[Insert::NoMate].second =
    m_query->unmatedCheck->isChecked();

  emit paintCanvas();
}

void InsertWindow::bankChanged()
{
  buildLibraryMenu();
}

QPixmap InsertWindow::mateIcon(const QColor & color,int mode)
{
  static const int height = 4;
  static const int readwidth = 30;
  static QPixmap mate (readwidth * 3, height);
  static const QRect cutout (readwidth,0,readwidth,height-1);
  static const QRect nomate (readwidth,0,readwidth*2,height-1);
  static const QRect nolink (readwidth,0,readwidth*2,height);

  QPainter p(&mate);
  p.fillRect(mate.rect(), color);
  if ( mode == 0 ) p.fillRect(cutout, paletteBackgroundColor());
  if ( mode == 1 ) p.fillRect(nomate, paletteBackgroundColor());
  if ( mode == 2 ) p.fillRect(nolink, paletteBackgroundColor());
  p.end();

  return mate;
}

void InsertWindow::setMaxInsertCovTol(int maxtol)
{
  m_query->insertSlider->setRange(0, maxtol);
  m_inserts->setInsertCovTol(m_query->insertSlider->value());
}

void InsertWindow::setMaxReadCovTol(int maxtol)
{
  m_query->readSlider->setRange(0, maxtol);
  m_inserts->setReadCovTol(m_query->readSlider->value());
}

void InsertWindow::setMaxSNPTol(int maxtol)
{
  m_query->snpSlider->setRange(0, maxtol);
  m_inserts->setSNPTol(m_query->snpSlider->value());
}

void InsertWindow::setMaxUnitigTol(int maxtol)
{
  m_query->unitigSlider->setRange(0, maxtol);
  m_inserts->setUnitigTol(m_query->unitigSlider->value());
}

void InsertWindow::setMaxQCTol(int maxtol)
{
  m_query->qcSlider->setRange(0, maxtol);
  m_inserts->setQCTol(m_query->qcSlider->value());
}

void InsertWindow::setMaxBreakTol(int maxtol)
{
  m_query->breakSlider->setRange(0, maxtol);
  m_inserts->setBreakTol(m_query->breakSlider->value());
}
