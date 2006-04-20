/****************************************************************************
** Form implementation generated from reading ui file 'QueryWidget.ui'
**
** Created: Wed Apr 19 20:08:21 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "QueryWidget.hh"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qslider.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a QueryWidget_t as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
QueryWidget_t::QueryWidget_t( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "QueryWidget_t" );
    QueryWidget_tLayout = new QVBoxLayout( this, 0, 0, "QueryWidget_tLayout"); 

    queryBox = new QGroupBox( this, "queryBox" );
    queryBox->setFlat( TRUE );
    queryBox->setColumnLayout(0, Qt::Vertical );
    queryBox->layout()->setSpacing( 6 );
    queryBox->layout()->setMargin( 11 );
    queryBoxLayout = new QGridLayout( queryBox->layout() );
    queryBoxLayout->setAlignment( Qt::AlignTop );

    snpSlider = new QSlider( queryBox, "snpSlider" );
    snpSlider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, snpSlider->sizePolicy().hasHeightForWidth() ) );
    snpSlider->setMinValue( 0 );
    snpSlider->setMaxValue( 10 );
    snpSlider->setValue( 0 );
    snpSlider->setOrientation( QSlider::Horizontal );
    snpSlider->setTickmarks( QSlider::Below );
    snpSlider->setTickInterval( 1 );

    queryBoxLayout->addWidget( snpSlider, 0, 1 );

    snpLabel = new QLabel( queryBox, "snpLabel" );
    snpLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, snpLabel->sizePolicy().hasHeightForWidth() ) );
    QFont snpLabel_font(  snpLabel->font() );
    snpLabel_font.setPointSize( 9 );
    snpLabel->setFont( snpLabel_font ); 
    snpLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    queryBoxLayout->addWidget( snpLabel, 0, 0 );

    kmerSlider = new QSlider( queryBox, "kmerSlider" );
    kmerSlider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, kmerSlider->sizePolicy().hasHeightForWidth() ) );
    kmerSlider->setMinValue( 0 );
    kmerSlider->setMaxValue( 10 );
    kmerSlider->setValue( 0 );
    kmerSlider->setOrientation( QSlider::Horizontal );
    kmerSlider->setTickmarks( QSlider::Below );
    kmerSlider->setTickInterval( 1 );

    queryBoxLayout->addWidget( kmerSlider, 3, 1 );

    kmerLabel = new QLabel( queryBox, "kmerLabel" );
    kmerLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, kmerLabel->sizePolicy().hasHeightForWidth() ) );
    QFont kmerLabel_font(  kmerLabel->font() );
    kmerLabel_font.setPointSize( 9 );
    kmerLabel->setFont( kmerLabel_font ); 

    queryBoxLayout->addWidget( kmerLabel, 3, 0 );

    covSlider = new QSlider( queryBox, "covSlider" );
    covSlider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, covSlider->sizePolicy().hasHeightForWidth() ) );
    covSlider->setMinValue( 0 );
    covSlider->setMaxValue( 10 );
    covSlider->setValue( 0 );
    covSlider->setOrientation( QSlider::Horizontal );
    covSlider->setTickmarks( QSlider::Below );
    covSlider->setTickInterval( 1 );

    queryBoxLayout->addWidget( covSlider, 2, 1 );

    covLabel = new QLabel( queryBox, "covLabel" );
    covLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, covLabel->sizePolicy().hasHeightForWidth() ) );
    QFont covLabel_font(  covLabel->font() );
    covLabel_font.setPointSize( 9 );
    covLabel->setFont( covLabel_font ); 
    covLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    queryBoxLayout->addWidget( covLabel, 2, 0 );

    ceSlider = new QSlider( queryBox, "ceSlider" );
    ceSlider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, ceSlider->sizePolicy().hasHeightForWidth() ) );
    ceSlider->setMinValue( 0 );
    ceSlider->setMaxValue( 10 );
    ceSlider->setValue( 0 );
    ceSlider->setOrientation( QSlider::Horizontal );
    ceSlider->setTickmarks( QSlider::Below );
    ceSlider->setTickInterval( 1 );

    queryBoxLayout->addWidget( ceSlider, 1, 1 );

    ceLabel = new QLabel( queryBox, "ceLabel" );
    ceLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, ceLabel->sizePolicy().hasHeightForWidth() ) );
    QFont ceLabel_font(  ceLabel->font() );
    ceLabel_font.setPointSize( 9 );
    ceLabel->setFont( ceLabel_font ); 
    ceLabel->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    queryBoxLayout->addWidget( ceLabel, 1, 0 );
    QueryWidget_tLayout->addWidget( queryBox );
    languageChange();
    resize( QSize(281, 369).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
QueryWidget_t::~QueryWidget_t()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QueryWidget_t::languageChange()
{
    setCaption( tr( "Form2" ) );
    queryBox->setTitle( tr( "Feature Queries" ) );
    snpLabel->setText( tr( "SNPs" ) );
    kmerLabel->setText( tr( "Kmer Freq" ) );
    covLabel->setText( tr( "Coverage" ) );
    ceLabel->setText( tr( "CE Stat" ) );
}

