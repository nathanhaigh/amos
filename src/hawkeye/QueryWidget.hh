/****************************************************************************
** Form interface generated from reading ui file 'QueryWidget.ui'
**
** Created by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QUERYWIDGET_H
#define QUERYWIDGET_H

#include <qvariant.h>
#include <qwidget.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QLabel>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QSpacerItem;
class Q3GroupBox;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;
class QSlider;
class Q3ButtonGroup;
class QRadioButton;

class QueryWidget : public QWidget
{
    Q_OBJECT

public:
    QueryWidget( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
    ~QueryWidget();

    Q3GroupBox* queryBox;
    QLineEdit* searchEdit;
    QPushButton* searchButton;
    QLineEdit* happyEdit;
    QPushButton* happyButton;
    Q3GroupBox* mateTypeBox;
    QCheckBox* singletonCheck;
    QCheckBox* unmatedCheck;
    QCheckBox* stretchedCheck;
    QCheckBox* linkingCheck;
    QCheckBox* compressedCheck;
    QCheckBox* happyCheck;
    QCheckBox* orientationCheck;
    QLabel* happyLabel;
    QLabel* stretchedLabel;
    QLabel* compressedLabel;
    QLabel* linkingLabel;
    QLabel* orientationLabel;
    QLabel* singletonLabel;
    QLabel* unmatedLabel;
    Q3GroupBox* featureBox;
    QLabel* allLabel;
    QCheckBox* readCheck;
    QLabel* noneLabel;
    QSlider* insertSlider;
    QSlider* readSlider;
    QCheckBox* insertCheck;
    QCheckBox* misasmCheck;
    QCheckBox* otherCheck;
    Q3ButtonGroup* mateColorBox;
    QRadioButton* categoricalButton;
    QRadioButton* continuousButton;
    QRadioButton* linkingButton;
    QRadioButton* libraryButton;
    Q3GroupBox* displayBox;
    QCheckBox* coverageCheck;
    QCheckBox* ceCheck;
    QCheckBox* mateCheck;
    QCheckBox* partitionCheck;
    QCheckBox* tintCheck;
    Q3GroupBox* libraryBox;

protected:
    Q3VBoxLayout* QueryWidgetLayout;
    Q3GridLayout* queryBoxLayout;
    Q3HBoxLayout* layout9;
    Q3HBoxLayout* layout8;
    Q3GridLayout* mateTypeBoxLayout;
    Q3VBoxLayout* featureBoxLayout;
    Q3GridLayout* sliderLayout;
    Q3VBoxLayout* mateColorBoxLayout;
    Q3VBoxLayout* displayBoxLayout;

protected slots:
    virtual void languageChange();

};

#endif // QUERYWIDGET_H
