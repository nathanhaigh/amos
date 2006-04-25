/****************************************************************************
** Form interface generated from reading ui file 'QueryWidget.ui'
**
** Created: Mon Apr 24 17:22:52 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QUERYWIDGET_H
#define QUERYWIDGET_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QFrame;
class QLineEdit;
class QPushButton;
class QSlider;
class QLabel;

class QueryWidget : public QWidget
{
    Q_OBJECT

public:
    QueryWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~QueryWidget();

    QGroupBox* queryBox;
    QFrame* line1;
    QLineEdit* happyEdit;
    QPushButton* happyButton;
    QLineEdit* searchEdit;
    QPushButton* searchButton;
    QSlider* insertSlider;
    QSlider* readSlider;
    QLabel* insertLabel;
    QLabel* readLabel;

protected:
    QVBoxLayout* QueryWidgetLayout;
    QGridLayout* queryBoxLayout;
    QHBoxLayout* layout8;
    QHBoxLayout* layout9;

protected slots:
    virtual void languageChange();

};

#endif // QUERYWIDGET_H
