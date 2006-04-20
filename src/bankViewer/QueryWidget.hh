/****************************************************************************
** Form interface generated from reading ui file 'QueryWidget.ui'
**
** Created: Wed Apr 19 22:13:31 2006
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
class QSpacerItem;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSlider;
class QFrame;

class QueryWidget : public QWidget
{
    Q_OBJECT

public:
    QueryWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~QueryWidget();

    QGroupBox* queryBox;
    QLabel* searchLabel;
    QLineEdit* searchEdit;
    QLabel* ceLabel;
    QLabel* covLabel;
    QLabel* kmerLabel;
    QSlider* ceSlider;
    QSlider* snpSlider;
    QLabel* snpLabel;
    QSlider* covSlider;
    QSlider* kmerSlider;
    QFrame* line1;
    QLabel* happyLabel;
    QLineEdit* happyEdit;

protected:
    QVBoxLayout* QueryWidgetLayout;
    QGridLayout* queryBoxLayout;
    QHBoxLayout* layout7;
    QHBoxLayout* layout2;

protected slots:
    virtual void languageChange();

};

#endif // QUERYWIDGET_H
