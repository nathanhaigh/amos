#include <iostream>
#include <qapplication.h>
#include <qsplashscreen.h>
#include <qpainter.h>

#include "MainWindow.hh"
#include "icons/AAI.xpm"

using namespace std;

int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  if (argc > 4)
  {
    cerr << "Usage: bankViewer [bankname [contigid [position]]]" << endl;
    exit (1);
  }

  QPixmap pixmap((const char **)AAI_xpm);
  
  if (argc > 1)
  {
    QPainter p (&pixmap);
    p.drawText(200, 200, 100, 48, Qt::AlignLeft | Qt::AlignBottom, "Waiting for Adam...");
    p.end();
  }

  QSplashScreen * splash = new QSplashScreen(pixmap);
  splash->show();

  MainWindow w(NULL, "mainwindow");
  w.setGeometry( 100, 100, 700, 355 );
  a.setMainWidget( &w );
  w.show();

  if (argc > 1)
  {
    string bankname = argv[1];
    w.setBankname(bankname);
    if (argc > 2)
    {
      w.setContigId(atoi(argv[2]));

      if (argc > 3)
      {
        w.setGindex(atoi(argv[3]));
      }
    }
  }

  sleep(1);

  splash->finish(&w);
  delete splash;

  return a.exec();
}
