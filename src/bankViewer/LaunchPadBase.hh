/****************************************************************************
** Form interface generated from reading ui file 'LaunchPadBase.ui'
**
** Created: Wed Apr 26 23:57:37 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

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
class QTabWidget;
class QWidget;
class QTextEdit;
class QListView;
class QListViewItem;
class QButtonGroup;
class QRadioButton;
class QPushButton;
class QLabel;
class QLineEdit;
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
    QWidget* featuresTab;
    QListView* featureList;
    QButtonGroup* groupGroup;
    QRadioButton* featureGroupTypeButton;
    QRadioButton* featureGroupContigButton;
    QButtonGroup* buttonGroup6;
    QPushButton* featureViewButton;
    QWidget* librariesTab;
    QListView* libraryList;
    QButtonGroup* histogramGroup;
    QPushButton* libraryInsertButton;
    QPushButton* libraryClearLengthButton;
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
    QButtonGroup* buttonGroup7;
    QPushButton* scaffoldViewButton;
    QWidget* contigsTab;
    QButtonGroup* buttonGroup7_2;
    QPushButton* contigViewButton;
    QLineEdit* contigEIDEdit;
    QLabel* textLabel1_2_2;
    QLabel* textLabel1_3;
    QButtonGroup* scaffoldHistogramGroup_2;
    QPushButton* contigLengthButton;
    QPushButton* contigReadsButton;
    QPushButton* contigGCButton;
    QLineEdit* contigIIDEdit;
    QListView* contigList;
    QWidget* TabPage;
    QLabel* textLabel1_3_2;
    QLineEdit* readEIDEdit;
    QLabel* textLabel1_2_2_2;
    QLineEdit* readIIDEdit;
    QButtonGroup* scaffoldHistogramGroup_2_2;
    QPushButton* readLengthButton;
    QPushButton* readGCButton;
    QButtonGroup* buttonGroup7_2_2;
    QPushButton* readViewButton;
    QListView* readList;
    QLabel* contigIDLabel;
    QSpinBox* contigIDSpin;
    QPushButton* insertButton;
    QPushButton* tilingButton;
    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QToolBar *Toolbar;
    QAction* fileOpenAction;
    QAction* fileQuitAction;
    QAction* fileChromatogramPathsAction;

public slots:
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();
    virtual void fileChromoPaths();

protected:
    QHBoxLayout* LaunchPadBaseLayout;
    QGridLayout* statsTabLayout;
    QGridLayout* featuresTabLayout;
    QSpacerItem* spacer1;
    QHBoxLayout* groupGroupLayout;
    QHBoxLayout* buttonGroup6Layout;
    QGridLayout* librariesTabLayout;
    QHBoxLayout* histogramGroupLayout;
    QGridLayout* scaffoldsTabLayout;
    QHBoxLayout* scaffoldHistogramGroupLayout;
    QHBoxLayout* buttonGroup7Layout;
    QGridLayout* contigsTabLayout;
    QHBoxLayout* buttonGroup7_2Layout;
    QHBoxLayout* scaffoldHistogramGroup_2Layout;
    QGridLayout* TabPageLayout;
    QGridLayout* scaffoldHistogramGroup_2_2Layout;
    QHBoxLayout* buttonGroup7_2_2Layout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // LAUNCHPADBASE_H
