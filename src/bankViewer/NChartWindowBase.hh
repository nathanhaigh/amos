
#ifndef NCHARTWINDOWBASE_H
#define NCHARTWINDOWBASE_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class NChartWidget;
class QFrame;
class QButtonGroup;
class QRadioButton;

class NChartWindowBase : public QMainWindow
{
    Q_OBJECT

public:
    NChartWindowBase( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~NChartWindowBase();

    NChartWidget* nChartWidget;
    QFrame* frame5;
    QButtonGroup* colorGroup;
    QRadioButton* greenRedColor;
    QRadioButton* redTintColor;
    QButtonGroup* orderingGroup;
    QRadioButton* sizeOrder;
    QRadioButton* featureOrder;
    QRadioButton* treemapOrder;

protected:
    QGridLayout* NChartWindowBaseLayout;
    QGridLayout* frame5Layout;
    QSpacerItem* spacer3;
    QGridLayout* colorGroupLayout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // NCHARTWINDOWBASE_H
