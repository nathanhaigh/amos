#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_ 1

#include <qwidget.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qscrollbar.h>
#include <string>

class MainWindow: public QWidget
{
  Q_OBJECT
public:
    MainWindow(QWidget *parent=0, const char *name=0 );

    void setBankname(string bankname);
    void setContigId(int contigID);
    void setGindex(int gindex);

public slots:
  void chooseBank();
  void setContigRange(int, int);
  void setGindexRange(int, int);

signals:
  void bankSelected(string bankname);
  void contigIdSelected(int contigId);
  void gindexChanged(int gindex);

private:
  QSpinBox * m_contigid;
  QSpinBox * m_gindex;
  QScrollBar * m_slider;
};

#endif
