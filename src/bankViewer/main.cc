#include <iostream>
#include <qapplication.h>
#include <qsplashscreen.h>
#include <qpainter.h>

#include "MainWindow.hh"
#include "icons/AAI.xpm"

using namespace std;

int main( int argc, char **argv )
{
  for (int i = 0; i < argc; i++)
  {
    if (!strcmp(argv[i], "-h"))
    {
      cerr << "Usage: bankViewer [options] [bankname [contigid [position]]]" << endl;
      cerr << "Options:"  << endl
           << "  -c <path>   Add a chromatogram path"      << endl
           << "  -d <dbpath> Add a chromatogram db path"   << endl
           << "  -D <DB>     Set the chromatogram DB"      << endl
           << "  -h          Display this help"            << endl;

      exit (1);
    }
  }

  QApplication a( argc, argv );

  QPixmap pixmap((const char **)AAI_xpm);
  
  if (a.argc() > 1)
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


  int state = 0;

  for (int i = 1; i < a.argc(); i++)
  {
    string v = a.argv()[i];

    if (v == "-c" && i+1 < a.argc())
    {
      i++; v = a.argv()[i];
      w.addChromoPath(v);
    }
    else if (v == "-d" && i+1 < a.argc())
    {
      i++; v = a.argv()[i];
      w.addChromoDB(v);
    }
    else if (v == "-D" && i+1 < a.argc())
    {
      i++; v = a.argv()[i];
      w.setChromoDB(QString(v));
    }
    else if (v[0] == '-')
    {
      cerr << "Option " << v << " not understood, ignoring" << endl;
    }
    else 
    {
      switch (state)
      {
        case 0: w.setBankname(v); break;
        case 1: w.setContigId(atoi(v.c_str())); break;
        case 2: w.setGindex(atoi(v.c_str())); break;
        default: cerr << "Value " << v << " not understood, ignoring" << endl;
      };

      state++;
    }
  }

  splash->finish(&w);
  delete splash;

  return a.exec();
}
