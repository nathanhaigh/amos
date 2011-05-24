#include <iostream>
//Added by qt3to4:
#include <QPixmap>
#include <QtGui>

#include "LaunchPad.hh"
#include "InsertWindow.hh"
#include "icons/AAI.xpm"
#include "icons/Hawkeye.xpm"
#include <qcursor.h>

#include "foundation_AMOS.hh"

#include "config.h"

#ifdef HAVE_QT_333
#include <qsplashscreen.h>
#else
#include "SplashScreen.hh"
#endif

using namespace std;

int main( int argc, char **argv )
{
  try
  {
    for (int i = 0; i < argc; i++)
    {
      if (!strcmp(argv[i], "-h"))
      {
        cerr << "Usage: hawkeye [options] [bankname [contigid [position]]]" << endl;
        cerr << "Options:"  << endl
             << "  -c <path>   Add a chromatogram path"        << endl
             << "  -D <DB>     Set the chromatogram DB"        << endl
             << "  -T          Enable Trace Fetch cmd"         << endl
             << "  -p <port>   Initialize Server on this port" << endl
             << "  -K <kmer>   Load File of kmers"             << endl
             << "  -h          Display this help"              << endl;

        exit (1);
      }
    }

    QApplication a( argc, argv );


    QPixmap pixmap((const char **)Hawkeye_xpm);

    int texttop =210;
    int textleft = 160;
    QPainter p (&pixmap);
    
    p.drawText(textleft, texttop, 200, 48, 
               Qt::AlignLeft | Qt::AlignBottom, 
               PACKAGE_STRING);

    if (a.argc() > 1)
    {
      p.drawText(textleft, texttop+20, 200, 48, 
                 Qt::AlignLeft | Qt::AlignBottom, 
                 "Loading assembly...");
    }
    p.end();

#ifdef HAVE_QT_333
    QSplashScreen * splash = new QSplashScreen(pixmap);
    splash->show();
#else
    SplashScreen * splash = new SplashScreen(pixmap);
#endif

    splash->setCursor(Qt::waitCursor);

    LaunchPad w(NULL, "launchpad");
    a.setMainWidget( &w );


    int state = 0;

    for (int i = 1; i < a.argc(); i++)
    {
      string v = a.argv()[i];

      if (v == "-c" && i+1 < a.argc())
      {
        i++; v = a.argv()[i];
        w.addChromoPath(QString(v.c_str()));
      }
      else if (v == "-p" && i+1 < a.argc())
      {
        i++; v = a.argv()[i];
        int port = atoi(v.c_str());
        w.initializeSimpleServer(port);
      }
      else if (v == "-D" && i+1 < a.argc())
      {
        i++; v = a.argv()[i];
        w.setChromoDB(QString(v.c_str()));
      }
      else if (v == "-K" && i+1 < a.argc())
      {
        i++; v = a.argv()[i];
        w.loadKmers(v);
      }
      else if (v == "-T")
      {
        w.enableTraceFetch(true);
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
    w.show();
    w.initDisplay();

    return a.exec();
  }
  catch (AMOS::Exception_t & e)
  {
    cerr << "ERROR: " << e << endl;
  }
  catch (...)
  {
    cerr << "Unexpected error" << endl;
  }

  return 1;
}
