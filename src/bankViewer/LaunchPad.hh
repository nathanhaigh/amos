/****************************************************************************
** Form interface generated from reading ui file 'LaunchPad.ui'
**
** Created: Wed Apr 26 14:59:48 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FORM3_H
#define FORM3_H

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

class Form3 : public QMainWindow
{
    Q_OBJECT

public:
    Form3( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~Form3();

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
    QLabel* textLabel1_2_2;
    QLabel* textLabel1_3;
    QListView* contigList;
    QButtonGroup* buttonGroup7_2;
    QPushButton* contigViewButton;
    QButtonGroup* scaffoldHistogramGroup_2;
    QPushButton* contigLengthButton;
    QPushButton* contigReadsButton;
    QPushButton* contigGCButton;
    QLineEdit* contigIIDEdit;
    QLineEdit* contigEIDEdit;
    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QAction* fileOpenAction;
    QAction* fileExitAction;
    QAction* fileChromatogramPathsAction;

public slots:
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();

protected:
    QHBoxLayout* Form3Layout;
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

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // FORM3_H
