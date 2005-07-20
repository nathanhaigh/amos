#include <qapplication.h>
#include <qpainter.h>
#include <qpixmap.h>
#include "SplashScreen.hh"

SplashScreen::SplashScreen( const QPixmap &pixmap )
    : QWidget( 0, 0, WStyle_Customize | WStyle_Splash ),
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

#if defined(Q_WS_X11)
void qt_wait_for_window_manager( QWidget *widget );
#endif

void SplashScreen::finish( QWidget *mainWin )
{
  #if defined(Q_WS_X11)
  qt_wait_for_window_manager( mainWin );
  #endif

  close();
}

void SplashScreen::setStatus( const QString &message, int alignment, const QColor &color )
{
  QPixmap textPix = pix;
  QPainter painter( &textPix, this );
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

