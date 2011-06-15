
#ifndef LAUNCHPADBASE_H
#define LAUNCHPADBASE_H

#include <qvariant.h>
#include <qpixmap.h>
#include <q3mainwindow.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <QLabel>
#include <Q3PopupMenu>
#include <Q3VBoxLayout>
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
class QTabWidget;
class QWidget;
class Q3TextEdit;
class Q3ButtonGroup;
class QRadioButton;
class Q3ListView;
class Q3ListViewItem;
class QPushButton;
class QLabel;
class QLineEdit;
class QCheckBox;
class QSpinBox;

class LaunchPadBase : public Q3MainWindow
{
    Q_OBJECT

public:
    LaunchPadBase( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = Qt::WType_TopLevel );
    ~LaunchPadBase();

    QTabWidget* tabWidget;
    QWidget* statsTab;
    Q3TextEdit* statsText;
    NChartWidget* scaffoldSizes;
    NChartWidget* contigSizes;
    Q3ButtonGroup* orderRadioGroup;
    QRadioButton* radioButton8;
    QRadioButton* radioButton9;
    QRadioButton* radioButton10;
    Q3ButtonGroup* colorRadioGroup;
    QRadioButton* radioButton11;
    QRadioButton* radioButton12;
    QWidget* featuresTab;
    Q3ListView* featureList;
    Q3ButtonGroup* groupGroup;
    QRadioButton* featureGroupContigButton;
    QRadioButton* featureGroupTypeButton;
    QWidget* librariesTab;
    Q3ListView* libraryList;
    Q3ButtonGroup* histogramGroup;
    QPushButton* libraryClearLengthButton;
    QPushButton* libraryInsertButton;
    QPushButton* libraryReadLengthButton;
    QPushButton* libraryGCButton;
    QWidget* scaffoldsTab;
    Q3ListView* scaffoldList;
    QLabel* textLabel1;
    QLabel* textLabel1_2;
    QLineEdit* scaffoldIIDEdit;
    QLineEdit* scaffoldEIDEdit;
    Q3ButtonGroup* scaffoldHistogramGroup;
    QPushButton* scaffoldSpanButton;
    QPushButton* scaffoldContigsButton;
    QWidget* contigsTab;
    QLabel* textLabel1_2_2;
    QLabel* textLabel1_3;
    Q3ButtonGroup* scaffoldHistogramGroup_2;
    QPushButton* contigLengthButton;
    QPushButton* contigReadsButton;
    QPushButton* contigCoverageButton;
    QPushButton* contigGCButton;
    Q3ListView* contigList;
    QLineEdit* contigEIDEdit;
    QLineEdit* contigIIDEdit;
    Q3ButtonGroup* buttonGroup7;
    QCheckBox* readsCheck;
    QWidget* TabPage;
    QLabel* textLabel1_3_2;
    QLineEdit* readEIDEdit;
    QLabel* textLabel1_2_2_2;
    QLineEdit* readIIDEdit;
    Q3ButtonGroup* scaffoldHistogramGroup_2_2;
    QPushButton* readLengthButton;
    QPushButton* readGCButton;
    Q3ListView* readList;
    QLabel* contigIDLabel;
    QSpinBox* contigIDSpin;
    QPushButton* viewButton;
    QPushButton* insertButton;
    QPushButton* tilingButton;
    QMenuBar *MenuBar;
    Q3PopupMenu *fileMenu;
    Q3ToolBar *Toolbar;
    QAction* fileOpenAction;
    QAction* fileQuitAction;
    QAction* fileChromatogramPathsAction;
    QAction* loadAmosFileAction;
    QAction* fileImportAction;
    QAction* loadKmersAction;

public slots:
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();
    virtual void fileChromoPaths();
    virtual void loadAmosFile();
    virtual void fileImport();
    virtual void loadKmersFile();

protected:
    Q3GridLayout* LaunchPadBaseLayout;
    Q3GridLayout* statsTabLayout;
    QSpacerItem* spacer7;
    Q3GridLayout* orderRadioGroupLayout;
    Q3GridLayout* colorRadioGroupLayout;
    Q3GridLayout* featuresTabLayout;
    QSpacerItem* spacer1;
    Q3HBoxLayout* groupGroupLayout;
    Q3GridLayout* librariesTabLayout;
    Q3GridLayout* histogramGroupLayout;
    Q3GridLayout* scaffoldsTabLayout;
    Q3HBoxLayout* scaffoldHistogramGroupLayout;
    Q3GridLayout* contigsTabLayout;
    Q3HBoxLayout* scaffoldHistogramGroup_2Layout;
    Q3VBoxLayout* buttonGroup7Layout;
    Q3GridLayout* TabPageLayout;
    Q3GridLayout* scaffoldHistogramGroup_2_2Layout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;

};

#endif // LAUNCHPADBASE_H
