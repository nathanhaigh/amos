/****************************************************************************
** Form interface generated from reading ui file 'DetailWidget.ui'
**
** Created: Wed Apr 19 20:06:07 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DETAILWIDGET_T_H
#define DETAILWIDGET_T_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QTextEdit;

class DetailWidget_t : public QWidget
{
    Q_OBJECT

public:
    DetailWidget_t( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DetailWidget_t();

    QGroupBox* detailBox;
    QTextEdit* detailText;

protected:
    QVBoxLayout* DetailWidget_tLayout;
    QVBoxLayout* detailBoxLayout;

protected slots:
    virtual void languageChange();

};

#endif // DETAILWIDGET_T_H
