
#ifndef DETAILWIDGET_H
#define DETAILWIDGET_H

#include <qvariant.h>
#include <qwidget.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class Q3GroupBox;
class Q3TextEdit;

class DetailWidget : public QWidget
{
    Q_OBJECT

public:
    DetailWidget( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
    ~DetailWidget();

    Q3GroupBox* detailBox;
    Q3TextEdit* detailText;

protected:
    Q3GridLayout* DetailWidgetLayout;
    Q3VBoxLayout* detailBoxLayout;

protected slots:
    virtual void languageChange();

};

#endif // DETAILWIDGET_H
