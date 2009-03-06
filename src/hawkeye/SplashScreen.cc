#include <qapplication.h>
#include <qpainter.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QMouseEvent>
#include "SplashScreen.hh"
#include <QtGui>

SplashScreen::SplashScreen( const QPixmap &pixmap )
    : QWidget( 0, 0, Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool | Qt::WWinOwnDC  ),
      pix( pixmap )
{
  setErasePixmap( pix );
  resize( pix.size() );
  QRect scr = QApplication::desktop()->screenGeometry();
  move( scr.center() - rect().center() );
  show();
  repaint();
}

void SplashScreen::repaint()
{
  QWidget::repaint();
  QApplication::flush();
}

void SplashScreen::finish( QWidget *mainWin )
{
  close();
}

void SplashScreen::setStatus( const QString &message, int alignment, const QColor &color )
{
  QPixmap textPix = pix;
  QPainter painter( &textPix);
  painter.setPen( color );
  QRect r = rect();
  r.setRect( r.x() + 10, r.y() + 10, r.width() - 20, r.height() - 20 );
  painter.drawText( r, alignment, message );
  setErasePixmap( textPix );
  repaint();
}

void SplashScreen::mousePressEvent( QMouseEvent * )
{
  hide();
}

