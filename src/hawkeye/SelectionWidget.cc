
#include "SelectionWidget.hh"

#include <qvariant.h>
#include <qpushbutton.h>
#include <q3groupbox.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3GridLayout>

/*
 *  Constructs a SelectionWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
SelectionWidget::SelectionWidget( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "SelectionWidget" );
    SelectionWidgetLayout = new Q3GridLayout( this, 1, 1, 0, 0, "SelectionWidgetLayout"); 

    groupBox7 = new Q3GroupBox( this, "groupBox7" );
    groupBox7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, groupBox7->sizePolicy().hasHeightForWidth() ) );
    groupBox7->setColumnLayout(0, Qt::Vertical );
    groupBox7->layout()->setSpacing( 2 );
    groupBox7->layout()->setMargin( 2 );
    groupBox7Layout = new Q3GridLayout( groupBox7->layout() );
    groupBox7Layout->setAlignment( Qt::AlignTop );

    zoomInButton = new QToolButton( groupBox7, "zoomInButton" );
    zoomInButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, zoomInButton->sizePolicy().hasHeightForWidth() ) );
    zoomInButton->setMinimumSize( QSize( 25, 25 ) );
    zoomInButton->setMaximumSize( QSize( 25, 25 ) );
    zoomInButton->setToggleButton( TRUE );

    groupBox7Layout->addWidget( zoomInButton, 0, 2 );

    zoomOutButton = new QToolButton( groupBox7, "zoomOutButton" );
    zoomOutButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, zoomOutButton->sizePolicy().hasHeightForWidth() ) );
    zoomOutButton->setMinimumSize( QSize( 25, 25 ) );
    zoomOutButton->setMaximumSize( QSize( 25, 25 ) );
    zoomOutButton->setToggleButton( TRUE );

    groupBox7Layout->addWidget( zoomOutButton, 0, 3 );

    syncWithTilingButton = new QCheckBox( groupBox7, "syncWithTilingButton" );
    syncWithTilingButton->setChecked( TRUE );

    groupBox7Layout->addWidget( syncWithTilingButton, 0, 5 );
    spacer2_2 = new QSpacerItem( 30, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox7Layout->addItem( spacer2_2, 0, 6 );

    selectButton = new QToolButton( groupBox7, "selectButton" );
    selectButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, selectButton->sizePolicy().hasHeightForWidth() ) );
    selectButton->setMinimumSize( QSize( 25, 25 ) );
    selectButton->setMaximumSize( QSize( 25, 25 ) );
    selectButton->setToggleButton( TRUE );
    selectButton->setOn( TRUE );

    groupBox7Layout->addWidget( selectButton, 0, 1 );
    spacer2 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    groupBox7Layout->addItem( spacer2, 0, 4 );
    spacer2_3 = new QSpacerItem( 16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    groupBox7Layout->addItem( spacer2_3, 0, 0 );

    SelectionWidgetLayout->addWidget( groupBox7, 0, 0 );
    languageChange();
    resize( QSize(347, 47).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SelectionWidget::~SelectionWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SelectionWidget::languageChange()
{
    setCaption( tr( "Selection Widget" ) );
    groupBox7->setTitle( tr( "Selection" ) );
    zoomInButton->setText( tr( "Zoom In" ) );
    QToolTip::add( zoomInButton, tr( "Zoom In" ) );
    zoomOutButton->setText( tr( "Zoom Out" ) );
    QToolTip::add( zoomOutButton, tr( "Zoom Out" ) );
    syncWithTilingButton->setText( tr( "Center on Tiling" ) );
    QToolTip::add( syncWithTilingButton, tr( "Center view on currently displayed region of contig." ) );
    selectButton->setText( tr( "Select" ) );
    QToolTip::add( selectButton, tr( "Select" ) );
}

