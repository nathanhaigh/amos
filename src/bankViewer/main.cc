/****************************************************************
**
** Qt tutorial 8
**
****************************************************************/

#include <qapplication.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qfont.h>
#include <qlayout.h>

#include "lcdrange.hh"
#include "TilingField.hh"


class MyWidget: public QWidget
{
public:
    MyWidget( string bankname, int contigID,
        QWidget *parent=0, const char *name=0 );
};


MyWidget::MyWidget( string bankname, int contigID,
                    QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    QPushButton *quit = new QPushButton( "&Quit", this, "quit" );
    quit->setFont( QFont( "Times", 18, QFont::Bold ) );

    connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );

    LCDRange * gindex = new LCDRange( this, "gindex" );
    TilingField * tiling = new TilingField( this, "tiling" );

    connect( gindex, SIGNAL(valueChanged(int)),
	     tiling, SLOT(setGindex(int)) );
    connect( tiling, SIGNAL(gindexChanged(int)),
	     gindex, SLOT(setValue(int)) );
    connect( tiling, SIGNAL(setRange(int, int)),
         gindex, SLOT(setRange(int, int)));

    tiling->loadContig(bankname, contigID);

    QGridLayout *grid = new QGridLayout( this, 2, 2, 10 );
    //2x2, 10 pixel border

    grid->addWidget( quit, 0, 0 );
    grid->addWidget( gindex, 0, 1);
    grid->addWidget( tiling, 1, 1 );
    grid->setColStretch( 1, 10 );

    gindex->setValue( 0 );
    gindex->setFocus();
}


int main( int argc, char **argv )
{
    if (argc != 3)
    {
      cerr << "Usage: bankViewer bankname contigid" << endl;
      exit (1);
    }

    QApplication a( argc, argv );

    MyWidget w(argv[1], atoi(argv[2]));
    w.setGeometry( 100, 100, 500, 355 );
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
