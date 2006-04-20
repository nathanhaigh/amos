/****************************************************************************
** Form interface generated from reading ui file 'QueryWidget.ui'
**
** Created: Wed Apr 19 20:08:11 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QUERYWIDGET_T_H
#define QUERYWIDGET_T_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QSlider;
class QLabel;

class QueryWidget_t : public QWidget
{
    Q_OBJECT

public:
    QueryWidget_t( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~QueryWidget_t();

    QGroupBox* queryBox;
    QSlider* snpSlider;
    QLabel* snpLabel;
    QSlider* kmerSlider;
    QLabel* kmerLabel;
    QSlider* covSlider;
    QLabel* covLabel;
    QSlider* ceSlider;
    QLabel* ceLabel;

protected:
    QVBoxLayout* QueryWidget_tLayout;
    QGridLayout* queryBoxLayout;

protected slots:
    virtual void languageChange();

};

#endif // QUERYWIDGET_T_H
