/****************************************************************************
** Form implementation generated from reading ui file 'DetailWidget.ui'
**
** Created: Wed Apr 19 20:06:26 2006
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
 *  Constructs a DetailWidget_t as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DetailWidget_t::DetailWidget_t( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DetailWidget_t" );
    DetailWidget_tLayout = new QVBoxLayout( this, 0, 0, "DetailWidget_tLayout"); 

    detailBox = new QGroupBox( this, "detailBox" );
    detailBox->setFlat( TRUE );
    detailBox->setColumnLayout(0, Qt::Vertical );
    detailBox->layout()->setSpacing( 0 );
    detailBox->layout()->setMargin( 0 );
    detailBoxLayout = new QVBoxLayout( detailBox->layout() );
    detailBoxLayout->setAlignment( Qt::AlignTop );

    detailText = new QTextEdit( detailBox, "detailText" );
    QFont detailText_font(  detailText->font() );
    detailText_font.setFamily( "Courier" );
    detailText_font.setPointSize( 8 );
    detailText->setFont( detailText_font ); 
    detailText->setCursor( QCursor( 4 ) );
    detailText->setWordWrap( QTextEdit::NoWrap );
    detailBoxLayout->addWidget( detailText );
    DetailWidget_tLayout->addWidget( detailBox );
    languageChange();
    resize( QSize(378, 473).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DetailWidget_t::~DetailWidget_t()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DetailWidget_t::languageChange()
{
    setCaption( tr( "Form2" ) );
    detailBox->setTitle( tr( "Selection Details" ) );
}

