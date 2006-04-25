/****************************************************************************
** Form implementation generated from reading ui file 'QueryWidget.ui'
**
** Created: Mon Apr 24 17:22:52 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "QueryWidget.hh"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a QueryWidget as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
QueryWidget::QueryWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "QueryWidget" );
    QueryWidgetLayout = new QVBoxLayout( this, 0, 0, "QueryWidgetLayout"); 

    queryBox = new QGroupBox( this, "queryBox" );
    queryBox->setFlat( TRUE );
    queryBox->setColumnLayout(0, Qt::Vertical );
    queryBox->layout()->setSpacing( 6 );
    queryBox->layout()->setMargin( 11 );
    queryBoxLayout = new QGridLayout( queryBox->layout() );
    queryBoxLayout->setAlignment( Qt::AlignTop );

    line1 = new QFrame( queryBox, "line1" );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );

    queryBoxLayout->addMultiCellWidget( line1, 2, 2, 0, 1 );

    layout8 = new QHBoxLayout( 0, 0, 6, "layout8"); 

    happyEdit = new QLineEdit( queryBox, "happyEdit" );
    happyEdit->setCursor( QCursor( 4 ) );
    happyEdit->setAlignment( int( QLineEdit::AlignRight ) );
    layout8->addWidget( happyEdit );

    happyButton = new QPushButton( queryBox, "happyButton" );
    happyButton->setMaximumSize( QSize( 130, 21 ) );
    QFont happyButton_font(  happyButton->font() );
    happyButton_font.setPointSize( 9 );
    happyButton->setFont( happyButton_font ); 
    layout8->addWidget( happyButton );

    queryBoxLayout->addMultiCellLayout( layout8, 1, 1, 0, 1 );

    layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 

    searchEdit = new QLineEdit( queryBox, "searchEdit" );
    searchEdit->setCursor( QCursor( 4 ) );
    searchEdit->setAlignment( int( QLineEdit::AlignLeft ) );
    layout9->addWidget( searchEdit );

    searchButton = new QPushButton( queryBox, "searchButton" );
    searchButton->setMaximumSize( QSize( 60, 21 ) );
    QFont searchButton_font(  searchButton->font() );
    searchButton_font.setPointSize( 9 );
    searchButton->setFont( searchButton_font ); 
    layout9->addWidget( searchButton );

    queryBoxLayout->addMultiCellLayout( layout9, 0, 0, 0, 1 );

    insertSlider = new QSlider( queryBox, "insertSlider" );
    insertSlider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, insertSlider->sizePolicy().hasHeightForWidth() ) );
    insertSlider->setMinValue( 0 );
    insertSlider->setMaxValue( 10 );
    insertSlider->setValue( 0 );
    insertSlider->setOrientation( QSlider::Horizontal );
    insertSlider->setTickmarks( QSlider::NoMarks );
    insertSlider->setTickInterval( 1 );

    queryBoxLayout->addWidget( insertSlider, 3, 1 );

    readSlider = new QSlider( queryBox, "readSlider" );
    readSlider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, readSlider->sizePolicy().hasHeightForWidth() ) );
    readSlider->setMinValue( 0 );
    readSlider->setMaxValue( 10 );
    readSlider->setValue( 0 );
    readSlider->setOrientation( QSlider::Horizontal );
    readSlider->setTickmarks( QSlider::NoMarks );
    readSlider->setTickInterval( 1 );

    queryBoxLayout->addWidget( readSlider, 4, 1 );

    insertLabel = new QLabel( queryBox, "insertLabel" );
    insertLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, insertLabel->sizePolicy().hasHeightForWidth() ) );
    QFont insertLabel_font(  insertLabel->font() );
    insertLabel_font.setPointSize( 9 );
    insertLabel->setFont( insertLabel_font ); 
    insertLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    queryBoxLayout->addWidget( insertLabel, 3, 0 );

    readLabel = new QLabel( queryBox, "readLabel" );
    readLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, readLabel->sizePolicy().hasHeightForWidth() ) );
    QFont readLabel_font(  readLabel->font() );
    readLabel_font.setPointSize( 9 );
    readLabel->setFont( readLabel_font ); 
    readLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    queryBoxLayout->addWidget( readLabel, 4, 0 );
    QueryWidgetLayout->addWidget( queryBox );
    languageChange();
    resize( QSize(243, 272).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( searchEdit, SIGNAL( returnPressed() ), searchButton, SLOT( animateClick() ) );
    connect( happyEdit, SIGNAL( returnPressed() ), happyButton, SLOT( animateClick() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
QueryWidget::~QueryWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QueryWidget::languageChange()
{
    setCaption( tr( "Form2" ) );
    queryBox->setTitle( tr( "Queries" ) );
    happyEdit->setText( tr( "2.0" ) );
    happyButton->setText( tr( "Happy Distance" ) );
    searchButton->setText( tr( "Search" ) );
    insertLabel->setText( tr( "Insert Cov" ) );
    readLabel->setText( tr( "Read Cov" ) );
}

