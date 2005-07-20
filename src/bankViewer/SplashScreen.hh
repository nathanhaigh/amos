#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <qpixmap.h>
#include <qwidget.h>

class SplashScreen : public QWidget
{
  Q_OBJECT

public:
    SplashScreen( const QPixmap &pixmap );

    void setStatus( const QString &message, int alignment = AlignLeft, const QColor &color = black );
    void finish( QWidget *mainWin );
    void repaint();

protected:
    void mousePressEvent( QMouseEvent * );

private:
    QPixmap pix;
};

#endif
