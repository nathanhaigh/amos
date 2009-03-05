
#include "DetailWidget.hh"

#include <qvariant.h>
#include <q3groupbox.h>
#include <q3textedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>

/*
 *  Constructs a DetailWidget as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DetailWidget::DetailWidget( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DetailWidget" );
    DetailWidgetLayout = new Q3GridLayout( this, 1, 1, 0, 0, "DetailWidgetLayout"); 

    detailBox = new Q3GroupBox( this, "detailBox" );
    detailBox->setFlat( TRUE );
    detailBox->setColumnLayout(0, Qt::Vertical );
    detailBox->layout()->setSpacing( 0 );
    detailBox->layout()->setMargin( 0 );
    detailBoxLayout = new Q3VBoxLayout( detailBox->layout() );
    detailBoxLayout->setAlignment( Qt::AlignTop );

    detailText = new Q3TextEdit( detailBox, "detailText" );
    detailText->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, detailText->sizePolicy().hasHeightForWidth() ) );
    QFont detailText_font(  detailText->font() );
    detailText_font.setFamily( "Courier" );
    detailText_font.setPointSize( 8 );
    detailText->setFont( detailText_font ); 
    detailText->setTextFormat( Qt::RichText );
    detailText->setWordWrap( Q3TextEdit::NoWrap );
    detailText->setReadOnly( TRUE );
    detailBoxLayout->addWidget( detailText );

    DetailWidgetLayout->addWidget( detailBox, 0, 0 );
    languageChange();
    resize( QSize(553, 537).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );
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
    detailBox->setTitle( tr( " Selection Details" ) );
}

