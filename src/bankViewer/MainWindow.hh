#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_ 1

#include <qwidget.h>
#include <qspinbox.h>
#include <qslider.h>
#include <string>

class MainWindow: public QWidget
{
  Q_OBJECT
public:
    MainWindow( string bankname, int contigID,
        QWidget *parent=0, const char *name=0 );

public slots:
  void openBank();
  void setContigRange(int, int);
  void setGindexRange(int, int);

signals:
  void bankSelected(string bankname);

private:
  QSpinBox * m_contigid;
  QSpinBox * m_gindex;
  QSlider * m_slider;


};

#endif
