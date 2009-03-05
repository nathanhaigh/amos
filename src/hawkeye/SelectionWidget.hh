
#ifndef SELECTIONWIDGET_H
#define SELECTIONWIDGET_H

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
class QToolButton;
class QCheckBox;

class SelectionWidget : public QWidget
{
    Q_OBJECT

public:
    SelectionWidget( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
    ~SelectionWidget();

    Q3GroupBox* groupBox7;
    QToolButton* zoomInButton;
    QToolButton* zoomOutButton;
    QCheckBox* syncWithTilingButton;
    QToolButton* selectButton;

protected:
    Q3GridLayout* SelectionWidgetLayout;
    Q3GridLayout* groupBox7Layout;
    QSpacerItem* spacer2_2;
    QSpacerItem* spacer2;
    QSpacerItem* spacer2_3;

protected slots:
    virtual void languageChange();

};

#endif // SELECTIONWIDGET_H
