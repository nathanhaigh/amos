/****************************************************************************
** Form interface generated from reading ui file 'QueryWidget.ui'
**
** Created: Tue Apr 25 14:06:26 2006
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
class QGroupBox;
class QLineEdit;
class QPushButton;
class QLabel;
class QSlider;
class QCheckBox;
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
    QGroupBox* featureBox;
    QLabel* noneLabel;
    QLabel* allLabel;
    QSlider* insertSlider;
    QSlider* readSlider;
    QSlider* snpSlider;
    QSlider* surrogateSlider;
    QCheckBox* surrogateCheck;
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
    QRadioButton* stretchButton;
    QRadioButton* linkingButton;
    QRadioButton* libraryButton;
    QGroupBox* mateTypeBox;
    QCheckBox* happyCheck;
    QCheckBox* stretchedCheck;
    QCheckBox* compressedCheck;
    QCheckBox* orientationCheck;
    QCheckBox* linkingCheck;
    QCheckBox* singletonCheck;
    QCheckBox* unmatedCheck;
    QGroupBox* displayBox;
    QCheckBox* coverageCheck;
    QCheckBox* ceCheck;
    QCheckBox* mateCheck;
    QCheckBox* partitionCheck;

protected:
    QVBoxLayout* QueryWidgetLayout;
    QGridLayout* queryBoxLayout;
    QHBoxLayout* layout9;
    QHBoxLayout* layout8;
    QGridLayout* featureBoxLayout;
    QVBoxLayout* mateTypeBoxLayout;

protected slots:
    virtual void languageChange();

};

#endif // QUERYWIDGET_H
