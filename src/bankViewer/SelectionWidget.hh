
#ifndef SELECTIONWIDGET_H
#define SELECTIONWIDGET_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QToolButton;
class QCheckBox;

class SelectionWidget : public QWidget
{
    Q_OBJECT

public:
    SelectionWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~SelectionWidget();

    QGroupBox* groupBox7;
    QToolButton* zoomInButton;
    QToolButton* zoomOutButton;
    QCheckBox* syncWithTilingButton;
    QToolButton* selectButton;

protected:
    QGridLayout* SelectionWidgetLayout;
    QGridLayout* groupBox7Layout;
    QSpacerItem* spacer2_2;
    QSpacerItem* spacer2;
    QSpacerItem* spacer2_3;

protected slots:
    virtual void languageChange();

};

#endif // SELECTIONWIDGET_H
