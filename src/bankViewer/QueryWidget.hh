/****************************************************************************
** Form interface generated from reading ui file 'QueryWidget.ui'
**
** Created: Fri Apr 28 17:58:11 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QUERYWIDGET_H
#define QUERYWIDGET_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;
class QSlider;
class QButtonGroup;
class QRadioButton;

class QueryWidget : public QWidget
{
    Q_OBJECT

public:
    QueryWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~QueryWidget();

    QGroupBox* queryBox;
    QLineEdit* searchEdit;
    QPushButton* searchButton;
    QLineEdit* happyEdit;
    QPushButton* happyButton;
    QGroupBox* mateTypeBox;
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
    QGroupBox* featureBox;
    QLabel* noneLabel;
    QLabel* allLabel;
    QSlider* insertSlider;
    QSlider* readSlider;
    QSlider* snpSlider;
    QSlider* unitigSlider;
    QCheckBox* unitigCheck;
    QCheckBox* snpCheck;
    QCheckBox* readCheck;
    QCheckBox* insertCheck;
    QCheckBox* qcCheck;
    QSlider* qcSlider;
    QSlider* breakSlider;
    QCheckBox* breakCheck;
    QCheckBox* otherCheck;
    QButtonGroup* mateColorBox;
    QRadioButton* categoricalButton;
    QRadioButton* continuousButton;
    QRadioButton* linkingButton;
    QRadioButton* libraryButton;
    QGroupBox* displayBox;
    QCheckBox* coverageCheck;
    QCheckBox* ceCheck;
    QCheckBox* mateCheck;
    QCheckBox* partitionCheck;
    QCheckBox* tintCheck;
    QGroupBox* libraryBox;

protected:
    QVBoxLayout* QueryWidgetLayout;
    QGridLayout* queryBoxLayout;
    QHBoxLayout* layout9;
    QHBoxLayout* layout8;
    QGridLayout* mateTypeBoxLayout;
    QGridLayout* featureBoxLayout;
    QVBoxLayout* mateColorBoxLayout;
    QVBoxLayout* displayBoxLayout;

protected slots:
    virtual void languageChange();

};

#endif // QUERYWIDGET_H
