
#include "NChartWindowBase.hh"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include "NChartWidget.hh"

/*
 *  Constructs a NChartWindowBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
NChartWindowBase::NChartWindowBase( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "NChartWindowBase" );
    setBaseSize( QSize( 500, 500 ) );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    NChartWindowBaseLayout = new QGridLayout( centralWidget(), 1, 1, 11, 6, "NChartWindowBaseLayout"); 

    nChartWidget = new NChartWidget( centralWidget(), "nChartWidget" );
    nChartWidget->setMinimumSize( QSize( 400, 400 ) );

    NChartWindowBaseLayout->addWidget( nChartWidget, 0, 0 );

    frame5 = new QFrame( centralWidget(), "frame5" );
    frame5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, frame5->sizePolicy().hasHeightForWidth() ) );
    frame5->setFrameShape( QFrame::StyledPanel );
    frame5->setFrameShadow( QFrame::Raised );
    frame5Layout = new QGridLayout( frame5, 1, 1, 11, 6, "frame5Layout"); 

    colorGroup = new QButtonGroup( frame5, "colorGroup" );
    colorGroup->setColumnLayout(0, Qt::Vertical );
    colorGroup->layout()->setSpacing( 6 );
    colorGroup->layout()->setMargin( 11 );
    colorGroupLayout = new QGridLayout( colorGroup->layout() );
    colorGroupLayout->setAlignment( Qt::AlignTop );

    greenRedColor = new QRadioButton( colorGroup, "greenRedColor" );
    greenRedColor->setChecked( TRUE );
    colorGroup->insert( greenRedColor, 0 );

    colorGroupLayout->addWidget( greenRedColor, 0, 0 );

    redTintColor = new QRadioButton( colorGroup, "redTintColor" );

    colorGroupLayout->addWidget( redTintColor, 1, 0 );

    frame5Layout->addWidget( colorGroup, 2, 0 );

    orderingGroup = new QButtonGroup( frame5, "orderingGroup" );

    sizeOrder = new QRadioButton( orderingGroup, "sizeOrder" );
    sizeOrder->setGeometry( QRect( 11, 22, 133, 21 ) );
    sizeOrder->setChecked( TRUE );

    featureOrder = new QRadioButton( orderingGroup, "featureOrder" );
    featureOrder->setGeometry( QRect( 11, 49, 133, 21 ) );

    treemapOrder = new QRadioButton( orderingGroup, "treemapOrder" );
    treemapOrder->setGeometry( QRect( 11, 76, 133, 21 ) );

    frame5Layout->addWidget( orderingGroup, 1, 0 );
    spacer3 = new QSpacerItem( 41, 81, QSizePolicy::Minimum, QSizePolicy::Expanding );
    frame5Layout->addItem( spacer3, 3, 0 );

    NChartWindowBaseLayout->addWidget( frame5, 0, 1 );

    // toolbars

    languageChange();
    resize( QSize(618, 455).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
NChartWindowBase::~NChartWindowBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void NChartWindowBase::languageChange()
{
    setCaption( tr( "NChartWindowBase" ) );
    colorGroup->setTitle( tr( "Coloring" ) );
    greenRedColor->setText( tr( "Green - Red" ) );
    redTintColor->setText( tr( "White - Red" ) );
    orderingGroup->setTitle( tr( "Ordering" ) );
    sizeOrder->setText( tr( "Size" ) );
    featureOrder->setText( tr( "Feature Density" ) );
    treemapOrder->setText( tr( "Treemap" ) );
}

