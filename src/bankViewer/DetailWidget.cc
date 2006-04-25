/****************************************************************************
** Form implementation generated from reading ui file 'DetailWidget.ui'
**
** Created: Mon Apr 24 17:22:52 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DetailWidget.hh"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DetailWidget as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
DetailWidget::DetailWidget( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DetailWidget" );
    DetailWidgetLayout = new QVBoxLayout( this, 0, 0, "DetailWidgetLayout"); 

    detailBox = new QGroupBox( this, "detailBox" );
    detailBox->setFlat( TRUE );
    detailBox->setColumnLayout(0, Qt::Vertical );
    detailBox->layout()->setSpacing( 6 );
    detailBox->layout()->setMargin( 11 );
    detailBoxLayout = new QVBoxLayout( detailBox->layout() );
    detailBoxLayout->setAlignment( Qt::AlignTop );

    detailText = new QTextEdit( detailBox, "detailText" );
    detailText->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, detailText->sizePolicy().hasHeightForWidth() ) );
    QFont detailText_font(  detailText->font() );
    detailText_font.setFamily( "Courier" );
    detailText_font.setPointSize( 8 );
    detailText->setFont( detailText_font ); 
    detailText->setTextFormat( QTextEdit::RichText );
    detailText->setWordWrap( QTextEdit::NoWrap );
    detailText->setReadOnly( TRUE );
    detailText->setTabStopWidth( 20 );
    detailBoxLayout->addWidget( detailText );
    DetailWidgetLayout->addWidget( detailBox );
    languageChange();
    resize( QSize(378, 473).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DetailWidget::~DetailWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DetailWidget::languageChange()
{
    setCaption( tr( "Form2" ) );
    detailBox->setTitle( tr( "Selection Details" ) );
}

