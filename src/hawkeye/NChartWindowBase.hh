
#ifndef NCHARTWINDOWBASE_H
#define NCHARTWINDOWBASE_H

#include <qvariant.h>
#include <qpixmap.h>
#include <q3mainwindow.h>
//Added by qt3to4:
#include <Q3PopupMenu>
#include <Q3VBoxLayout>
#include <Q3Frame>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3ActionGroup>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class QAction;
class Q3ActionGroup;
class Q3ToolBar;
class Q3PopupMenu;
class NChartWidget;
class Q3Frame;
class Q3ButtonGroup;
class QRadioButton;

class NChartWindowBase : public Q3MainWindow
{
    Q_OBJECT

public:
    NChartWindowBase( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = Qt::WType_TopLevel );
    ~NChartWindowBase();

    NChartWidget* nChartWidget;
    Q3Frame* frame5;
    Q3ButtonGroup* colorGroup;
    QRadioButton* greenRedColor;
    QRadioButton* redTintColor;
    Q3ButtonGroup* orderingGroup;
    QRadioButton* sizeOrder;
    QRadioButton* featureOrder;
    QRadioButton* treemapOrder;

protected:
    Q3GridLayout* NChartWindowBaseLayout;
    Q3GridLayout* frame5Layout;
    QSpacerItem* spacer3;
    Q3GridLayout* colorGroupLayout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // NCHARTWINDOWBASE_H
