/****************************************************************
**
** Implementation of LCDRange class, Qt tutorial 8
**
****************************************************************/

#include "lcdrange.hh"

#include <qslider.h>
#include <qlcdnumber.h>

LCDRange::LCDRange( QWidget *parent, const char *name )
        : QVBox( parent, name )
{
    QLCDNumber *lcd  = new QLCDNumber( 10, this, "lcd"  );
    slider = new QSlider( Horizontal, this, "slider" );
    slider->setValue( 0 );
    connect( slider, SIGNAL(valueChanged(int)),
	     lcd, SLOT(display(int)) );
    connect( slider, SIGNAL(valueChanged(int)),
	     SIGNAL(valueChanged(int)) );

    setFocusProxy( slider );
}

int LCDRange::value() const
{
  return slider->value();
}

void LCDRange::setValue( int value )
{
  slider->setValue( value );
}

void LCDRange::setRange( int minVal, int maxVal )
{
  cerr << "setting range to " << minVal  << " " << maxVal << endl;
  slider->setRange( minVal, maxVal );
}
