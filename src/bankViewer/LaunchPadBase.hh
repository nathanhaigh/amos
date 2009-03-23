
#ifndef LAUNCHPADBASE_H
#define LAUNCHPADBASE_H

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
class QTabWidget;
class QWidget;
class QTextEdit;
class QButtonGroup;
class QRadioButton;
class QListView;
class QListViewItem;
class QPushButton;
class QLabel;
class QLineEdit;
class QCheckBox;
class QSpinBox;

class LaunchPadBase : public QMainWindow
{
    Q_OBJECT

public:
    LaunchPadBase( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~LaunchPadBase();

    QTabWidget* tabWidget;
    QWidget* statsTab;
    QTextEdit* statsText;
    NChartWidget* scaffoldSizes;
    NChartWidget* contigSizes;
    QButtonGroup* orderRadioGroup;
    QRadioButton* radioButton8;
    QRadioButton* radioButton9;
    QRadioButton* radioButton10;
    QButtonGroup* colorRadioGroup;
    QRadioButton* radioButton11;
    QRadioButton* radioButton12;
    QWidget* featuresTab;
    QListView* featureList;
    QButtonGroup* groupGroup;
    QRadioButton* featureGroupContigButton;
    QRadioButton* featureGroupTypeButton;
    QWidget* librariesTab;
    QListView* libraryList;
    QButtonGroup* histogramGroup;
    QPushButton* libraryClearLengthButton;
    QPushButton* libraryInsertButton;
    QPushButton* libraryReadLengthButton;
    QPushButton* libraryGCButton;
    QWidget* scaffoldsTab;
    QListView* scaffoldList;
    QLabel* textLabel1;
    QLabel* textLabel1_2;
    QLineEdit* scaffoldIIDEdit;
    QLineEdit* scaffoldEIDEdit;
    QButtonGroup* scaffoldHistogramGroup;
    QPushButton* scaffoldSpanButton;
    QPushButton* scaffoldContigsButton;
    QWidget* contigsTab;
    QLabel* textLabel1_2_2;
    QLabel* textLabel1_3;
    QButtonGroup* scaffoldHistogramGroup_2;
    QPushButton* contigLengthButton;
    QPushButton* contigReadsButton;
    QPushButton* contigGCButton;
    QListView* contigList;
    QLineEdit* contigEIDEdit;
    QLineEdit* contigIIDEdit;
    QButtonGroup* buttonGroup7;
    QCheckBox* readsCheck;
    QWidget* TabPage;
    QLabel* textLabel1_3_2;
    QLineEdit* readEIDEdit;
    QLabel* textLabel1_2_2_2;
    QLineEdit* readIIDEdit;
    QButtonGroup* scaffoldHistogramGroup_2_2;
    QPushButton* readLengthButton;
    QPushButton* readGCButton;
    QListView* readList;
    QLabel* contigIDLabel;
    QSpinBox* contigIDSpin;
    QPushButton* viewButton;
    QPushButton* insertButton;
    QPushButton* tilingButton;
    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QToolBar *Toolbar;
    QAction* fileOpenAction;
    QAction* fileQuitAction;
    QAction* fileChromatogramPathsAction;
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
    virtual void fileImport();
    virtual void loadKmersFile();

protected:
    QGridLayout* LaunchPadBaseLayout;
    QGridLayout* statsTabLayout;
    QSpacerItem* spacer7;
    QGridLayout* orderRadioGroupLayout;
    QGridLayout* colorRadioGroupLayout;
    QGridLayout* featuresTabLayout;
    QSpacerItem* spacer1;
    QHBoxLayout* groupGroupLayout;
    QGridLayout* librariesTabLayout;
    QGridLayout* histogramGroupLayout;
    QGridLayout* scaffoldsTabLayout;
    QHBoxLayout* scaffoldHistogramGroupLayout;
    QGridLayout* contigsTabLayout;
    QHBoxLayout* scaffoldHistogramGroup_2Layout;
    QVBoxLayout* buttonGroup7Layout;
    QGridLayout* TabPageLayout;
    QGridLayout* scaffoldHistogramGroup_2_2Layout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;

};

#endif // LAUNCHPADBASE_H
