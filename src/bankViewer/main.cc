#include <qapplication.h>
#include "MainWindow.hh"

int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  string bankname;
  int contigid = 1;

  if (argc > 1)
  {
    bankname = argv[1];
    if (argc > 2)
    {
      contigid = atoi(argv[2]);

      if (argc > 3)
      {
        cerr << "Usage: bankViewer [bankname [contigid]]" << endl;
        exit (1);
      }
    }
  }

  MainWindow w(bankname, contigid, NULL, "mainwindow");
  w.setGeometry( 100, 100, 700, 355 );
  a.setMainWidget( &w );
  w.show();
  return a.exec();
}
