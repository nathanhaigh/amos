#include <qapplication.h>
#include "MainWindow.hh"

int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  if (argc > 4)
  {
    cerr << "Usage: bankViewer [bankname [contigid]]" << endl;
    exit (1);
  }

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

  return a.exec();
}
