/****************************************************************************
** Form implementation generated from reading ui file 'LaunchPad.ui'
**
** Created: Wed Apr 26 15:05:00 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "LaunchPad.hh"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qtextedit.h>
#include <qheader.h>
#include <qlistview.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

static const unsigned char image0_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x99, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xed, 0x94, 0x41, 0x0e, 0x85,
    0x20, 0x0c, 0x44, 0x5f, 0x89, 0xc7, 0x36, 0x7f, 0x61, 0xbc, 0x77, 0x5d,
    0x28, 0x48, 0xa4, 0x28, 0x60, 0xff, 0xce, 0xd9, 0x54, 0x8b, 0xbe, 0x8e,
    0x13, 0x04, 0x3e, 0x1d, 0x92, 0x81, 0x77, 0xf4, 0x81, 0xa1, 0x23, 0xdc,
    0x2b, 0x34, 0xf6, 0xf4, 0x7a, 0x3d, 0xe2, 0xb8, 0x65, 0xa8, 0x84, 0x3f,
    0x40, 0x01, 0x98, 0x2a, 0x0b, 0x3d, 0x5f, 0x62, 0xc5, 0x83, 0x00, 0xaa,
    0x1a, 0xd7, 0x05, 0x50, 0x44, 0x9a, 0xb9, 0xd5, 0x07, 0xa7, 0x73, 0xa8,
    0xa4, 0xba, 0x4f, 0x92, 0xa2, 0xdf, 0x33, 0x3c, 0x64, 0xc6, 0x3b, 0xeb,
    0xbd, 0x82, 0xe5, 0xb8, 0xad, 0xde, 0xcb, 0xcc, 0x78, 0x20, 0xeb, 0x42,
    0x66, 0xc6, 0x39, 0x74, 0x5d, 0xfa, 0x80, 0xf3, 0x6f, 0xaf, 0x66, 0xc6,
    0x6f, 0xa1, 0x9c, 0x3f, 0x88, 0x2f, 0xb4, 0x70, 0xec, 0x05, 0xcd, 0xc0,
    0xbe, 0xd0, 0x78, 0x93, 0xf6, 0x8e, 0x17, 0x14, 0x92, 0x63, 0x5f, 0x68,
    0x6c, 0x3e, 0xef, 0xf6, 0xba, 0x3c, 0x8f, 0xdd, 0x36, 0x6d, 0xc4, 0xc0,
    0x45, 0x2c, 0xf1, 0x53, 0xa9, 0x59, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};


/*
 *  Constructs a LaunchPad as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
LaunchPad::LaunchPad( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    QImage img;
    img.loadFromData( image0_data, sizeof( image0_data ), "PNG" );
    image0 = img;
    if ( !name )
	setName( "LaunchPad" );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    LaunchPadLayout = new QHBoxLayout( centralWidget(), 11, 6, "LaunchPadLayout"); 

    tabWidget = new QTabWidget( centralWidget(), "tabWidget" );

    statsTab = new QWidget( tabWidget, "statsTab" );
    statsTabLayout = new QGridLayout( statsTab, 1, 1, 11, 6, "statsTabLayout"); 

    statsText = new QTextEdit( statsTab, "statsText" );
    statsText->setTextFormat( QTextEdit::RichText );
    statsText->setReadOnly( TRUE );

    statsTabLayout->addWidget( statsText, 0, 0 );
    tabWidget->insertTab( statsTab, QString::fromLatin1("") );

    featuresTab = new QWidget( tabWidget, "featuresTab" );
    featuresTabLayout = new QGridLayout( featuresTab, 1, 1, 11, 6, "featuresTabLayout"); 

    featureList = new QListView( featuresTab, "featureList" );
    featureList->setAllColumnsShowFocus( TRUE );
    featureList->setShowSortIndicator( TRUE );
    featureList->setRootIsDecorated( TRUE );

    featuresTabLayout->addMultiCellWidget( featureList, 0, 0, 0, 2 );

    groupGroup = new QButtonGroup( featuresTab, "groupGroup" );
    groupGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, groupGroup->sizePolicy().hasHeightForWidth() ) );
    groupGroup->setColumnLayout(0, Qt::Vertical );
    groupGroup->layout()->setSpacing( 6 );
    groupGroup->layout()->setMargin( 11 );
    groupGroupLayout = new QHBoxLayout( groupGroup->layout() );
    groupGroupLayout->setAlignment( Qt::AlignTop );

    featureGroupTypeButton = new QRadioButton( groupGroup, "featureGroupTypeButton" );
    featureGroupTypeButton->setChecked( TRUE );
    groupGroupLayout->addWidget( featureGroupTypeButton );

    featureGroupContigButton = new QRadioButton( groupGroup, "featureGroupContigButton" );
    groupGroupLayout->addWidget( featureGroupContigButton );

    featuresTabLayout->addWidget( groupGroup, 1, 0 );

    buttonGroup6 = new QButtonGroup( featuresTab, "buttonGroup6" );
    buttonGroup6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, buttonGroup6->sizePolicy().hasHeightForWidth() ) );
    buttonGroup6->setColumnLayout(0, Qt::Vertical );
    buttonGroup6->layout()->setSpacing( 6 );
    buttonGroup6->layout()->setMargin( 11 );
    buttonGroup6Layout = new QHBoxLayout( buttonGroup6->layout() );
    buttonGroup6Layout->setAlignment( Qt::AlignTop );

    featureViewButton = new QPushButton( buttonGroup6, "featureViewButton" );
    featureViewButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, featureViewButton->sizePolicy().hasHeightForWidth() ) );
    buttonGroup6Layout->addWidget( featureViewButton );

    featuresTabLayout->addWidget( buttonGroup6, 1, 2 );
    spacer1 = new QSpacerItem( 190, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    featuresTabLayout->addItem( spacer1, 1, 1 );
    tabWidget->insertTab( featuresTab, QString::fromLatin1("") );

    librariesTab = new QWidget( tabWidget, "librariesTab" );
    librariesTabLayout = new QGridLayout( librariesTab, 1, 1, 11, 6, "librariesTabLayout"); 

    libraryList = new QListView( librariesTab, "libraryList" );
    libraryList->setAllColumnsShowFocus( TRUE );
    libraryList->setShowSortIndicator( TRUE );
    libraryList->setRootIsDecorated( TRUE );

    librariesTabLayout->addWidget( libraryList, 0, 0 );

    histogramGroup = new QButtonGroup( librariesTab, "histogramGroup" );
    histogramGroup->setColumnLayout(0, Qt::Vertical );
    histogramGroup->layout()->setSpacing( 6 );
    histogramGroup->layout()->setMargin( 11 );
    histogramGroupLayout = new QHBoxLayout( histogramGroup->layout() );
    histogramGroupLayout->setAlignment( Qt::AlignTop );

    libraryInsertButton = new QPushButton( histogramGroup, "libraryInsertButton" );
    libraryInsertButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, libraryInsertButton->sizePolicy().hasHeightForWidth() ) );
    histogramGroupLayout->addWidget( libraryInsertButton );

    libraryClearLengthButton = new QPushButton( histogramGroup, "libraryClearLengthButton" );
    libraryClearLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, libraryClearLengthButton->sizePolicy().hasHeightForWidth() ) );
    histogramGroupLayout->addWidget( libraryClearLengthButton );

    libraryReadLengthButton = new QPushButton( histogramGroup, "libraryReadLengthButton" );
    libraryReadLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, libraryReadLengthButton->sizePolicy().hasHeightForWidth() ) );
    histogramGroupLayout->addWidget( libraryReadLengthButton );

    libraryGCButton = new QPushButton( histogramGroup, "libraryGCButton" );
    libraryGCButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, libraryGCButton->sizePolicy().hasHeightForWidth() ) );
    histogramGroupLayout->addWidget( libraryGCButton );

    librariesTabLayout->addWidget( histogramGroup, 1, 0 );
    tabWidget->insertTab( librariesTab, QString::fromLatin1("") );

    scaffoldsTab = new QWidget( tabWidget, "scaffoldsTab" );
    scaffoldsTabLayout = new QGridLayout( scaffoldsTab, 1, 1, 11, 6, "scaffoldsTabLayout"); 

    scaffoldList = new QListView( scaffoldsTab, "scaffoldList" );
    scaffoldList->setAllColumnsShowFocus( TRUE );
    scaffoldList->setShowSortIndicator( TRUE );
    scaffoldList->setRootIsDecorated( TRUE );

    scaffoldsTabLayout->addMultiCellWidget( scaffoldList, 0, 0, 0, 3 );

    textLabel1 = new QLabel( scaffoldsTab, "textLabel1" );
    textLabel1->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    scaffoldsTabLayout->addWidget( textLabel1, 1, 0 );

    textLabel1_2 = new QLabel( scaffoldsTab, "textLabel1_2" );
    textLabel1_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    scaffoldsTabLayout->addWidget( textLabel1_2, 2, 0 );

    scaffoldIIDEdit = new QLineEdit( scaffoldsTab, "scaffoldIIDEdit" );
    scaffoldIIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, scaffoldIIDEdit->sizePolicy().hasHeightForWidth() ) );

    scaffoldsTabLayout->addWidget( scaffoldIIDEdit, 1, 1 );

    scaffoldEIDEdit = new QLineEdit( scaffoldsTab, "scaffoldEIDEdit" );
    scaffoldEIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, scaffoldEIDEdit->sizePolicy().hasHeightForWidth() ) );

    scaffoldsTabLayout->addWidget( scaffoldEIDEdit, 2, 1 );

    scaffoldHistogramGroup = new QButtonGroup( scaffoldsTab, "scaffoldHistogramGroup" );
    scaffoldHistogramGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, scaffoldHistogramGroup->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup->setColumnLayout(0, Qt::Vertical );
    scaffoldHistogramGroup->layout()->setSpacing( 6 );
    scaffoldHistogramGroup->layout()->setMargin( 11 );
    scaffoldHistogramGroupLayout = new QHBoxLayout( scaffoldHistogramGroup->layout() );
    scaffoldHistogramGroupLayout->setAlignment( Qt::AlignTop );

    scaffoldSpanButton = new QPushButton( scaffoldHistogramGroup, "scaffoldSpanButton" );
    scaffoldSpanButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, scaffoldSpanButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroupLayout->addWidget( scaffoldSpanButton );

    scaffoldContigsButton = new QPushButton( scaffoldHistogramGroup, "scaffoldContigsButton" );
    scaffoldContigsButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, scaffoldContigsButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroupLayout->addWidget( scaffoldContigsButton );

    scaffoldsTabLayout->addMultiCellWidget( scaffoldHistogramGroup, 1, 2, 2, 2 );

    buttonGroup7 = new QButtonGroup( scaffoldsTab, "buttonGroup7" );
    buttonGroup7->setColumnLayout(0, Qt::Vertical );
    buttonGroup7->layout()->setSpacing( 6 );
    buttonGroup7->layout()->setMargin( 11 );
    buttonGroup7Layout = new QHBoxLayout( buttonGroup7->layout() );
    buttonGroup7Layout->setAlignment( Qt::AlignTop );

    scaffoldViewButton = new QPushButton( buttonGroup7, "scaffoldViewButton" );
    scaffoldViewButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, scaffoldViewButton->sizePolicy().hasHeightForWidth() ) );
    buttonGroup7Layout->addWidget( scaffoldViewButton );

    scaffoldsTabLayout->addMultiCellWidget( buttonGroup7, 1, 2, 3, 3 );
    tabWidget->insertTab( scaffoldsTab, QString::fromLatin1("") );

    contigsTab = new QWidget( tabWidget, "contigsTab" );
    contigsTabLayout = new QGridLayout( contigsTab, 1, 1, 11, 6, "contigsTabLayout"); 

    textLabel1_2_2 = new QLabel( contigsTab, "textLabel1_2_2" );
    textLabel1_2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    contigsTabLayout->addWidget( textLabel1_2_2, 2, 0 );

    textLabel1_3 = new QLabel( contigsTab, "textLabel1_3" );
    textLabel1_3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    contigsTabLayout->addWidget( textLabel1_3, 1, 0 );

    contigList = new QListView( contigsTab, "contigList" );
    contigList->setAllColumnsShowFocus( TRUE );
    contigList->setShowSortIndicator( TRUE );
    contigList->setRootIsDecorated( TRUE );

    contigsTabLayout->addMultiCellWidget( contigList, 0, 0, 0, 3 );

    buttonGroup7_2 = new QButtonGroup( contigsTab, "buttonGroup7_2" );
    buttonGroup7_2->setColumnLayout(0, Qt::Vertical );
    buttonGroup7_2->layout()->setSpacing( 6 );
    buttonGroup7_2->layout()->setMargin( 11 );
    buttonGroup7_2Layout = new QHBoxLayout( buttonGroup7_2->layout() );
    buttonGroup7_2Layout->setAlignment( Qt::AlignTop );

    contigViewButton = new QPushButton( buttonGroup7_2, "contigViewButton" );
    contigViewButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, contigViewButton->sizePolicy().hasHeightForWidth() ) );
    buttonGroup7_2Layout->addWidget( contigViewButton );

    contigsTabLayout->addMultiCellWidget( buttonGroup7_2, 1, 2, 3, 3 );

    scaffoldHistogramGroup_2 = new QButtonGroup( contigsTab, "scaffoldHistogramGroup_2" );
    scaffoldHistogramGroup_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, scaffoldHistogramGroup_2->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2->setColumnLayout(0, Qt::Vertical );
    scaffoldHistogramGroup_2->layout()->setSpacing( 6 );
    scaffoldHistogramGroup_2->layout()->setMargin( 11 );
    scaffoldHistogramGroup_2Layout = new QHBoxLayout( scaffoldHistogramGroup_2->layout() );
    scaffoldHistogramGroup_2Layout->setAlignment( Qt::AlignTop );

    contigLengthButton = new QPushButton( scaffoldHistogramGroup_2, "contigLengthButton" );
    contigLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, contigLengthButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigLengthButton );

    contigReadsButton = new QPushButton( scaffoldHistogramGroup_2, "contigReadsButton" );
    contigReadsButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, contigReadsButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigReadsButton );

    contigGCButton = new QPushButton( scaffoldHistogramGroup_2, "contigGCButton" );
    contigGCButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, contigGCButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigGCButton );

    contigsTabLayout->addMultiCellWidget( scaffoldHistogramGroup_2, 1, 2, 2, 2 );

    contigIIDEdit = new QLineEdit( contigsTab, "contigIIDEdit" );
    contigIIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, contigIIDEdit->sizePolicy().hasHeightForWidth() ) );

    contigsTabLayout->addWidget( contigIIDEdit, 1, 1 );

    contigEIDEdit = new QLineEdit( contigsTab, "contigEIDEdit" );
    contigEIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, contigEIDEdit->sizePolicy().hasHeightForWidth() ) );

    contigsTabLayout->addWidget( contigEIDEdit, 2, 1 );
    tabWidget->insertTab( contigsTab, QString::fromLatin1("") );
    LaunchPadLayout->addWidget( tabWidget );

    // actions
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( image0 ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    fileChromatogramPathsAction = new QAction( this, "fileChromatogramPathsAction" );


    // toolbars


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );


    fileMenu = new QPopupMenu( this );
    fileOpenAction->addTo( fileMenu );
    fileChromatogramPathsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 1 );

    languageChange();
    resize( QSize(626, 598).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
LaunchPad::~LaunchPad()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void LaunchPad::languageChange()
{
    setCaption( tr( "Assembly Investigator" ) );
    tabWidget->changeTab( statsTab, tr( "Statistics" ) );
    groupGroup->setTitle( tr( "Group by" ) );
    featureGroupTypeButton->setText( tr( "Type" ) );
    featureGroupContigButton->setText( tr( "Contig" ) );
    buttonGroup6->setTitle( tr( "Selection" ) );
    featureViewButton->setText( tr( "View" ) );
    tabWidget->changeTab( featuresTab, tr( "Features" ) );
    histogramGroup->setTitle( tr( "Histograms" ) );
    libraryInsertButton->setText( tr( "Insert Size" ) );
    libraryClearLengthButton->setText( tr( "Read Clear Range" ) );
    libraryReadLengthButton->setText( tr( "Read Length" ) );
    libraryGCButton->setText( tr( "GC Content" ) );
    tabWidget->changeTab( librariesTab, tr( "Libraries" ) );
    textLabel1->setText( tr( "IID:" ) );
    textLabel1_2->setText( tr( "EID:" ) );
    scaffoldHistogramGroup->setTitle( tr( "Histograms" ) );
    scaffoldSpanButton->setText( tr( "Span" ) );
    scaffoldContigsButton->setText( tr( "Contig Count" ) );
    buttonGroup7->setTitle( tr( "Selection" ) );
    scaffoldViewButton->setText( tr( "View" ) );
    tabWidget->changeTab( scaffoldsTab, tr( "Scaffolds" ) );
    textLabel1_2_2->setText( tr( "EID:" ) );
    textLabel1_3->setText( tr( "IID:" ) );
    buttonGroup7_2->setTitle( tr( "Selection" ) );
    contigViewButton->setText( tr( "View" ) );
    scaffoldHistogramGroup_2->setTitle( tr( "Histograms" ) );
    contigLengthButton->setText( tr( "Length" ) );
    contigReadsButton->setText( tr( "Read Count" ) );
    contigGCButton->setText( tr( "GC Content" ) );
    tabWidget->changeTab( contigsTab, tr( "Contigs" ) );
    fileOpenAction->setText( tr( "Open" ) );
    fileOpenAction->setMenuText( tr( "&Open..." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    fileExitAction->setAccel( tr( "Ctrl+Q" ) );
    fileChromatogramPathsAction->setText( tr( "Chromatogram Paths" ) );
    fileChromatogramPathsAction->setMenuText( tr( "Chromatogram Paths" ) );
    fileChromatogramPathsAction->setAccel( tr( "Ctrl+P" ) );
    if (MenuBar->findItem(1))
        MenuBar->findItem(1)->setText( tr( "&File" ) );
}

void LaunchPad::fileNew()
{
    qWarning( "LaunchPad::fileNew(): Not implemented yet" );
}

void LaunchPad::fileOpen()
{
    qWarning( "LaunchPad::fileOpen(): Not implemented yet" );
}

void LaunchPad::fileSave()
{
    qWarning( "LaunchPad::fileSave(): Not implemented yet" );
}

void LaunchPad::fileSaveAs()
{
    qWarning( "LaunchPad::fileSaveAs(): Not implemented yet" );
}

void LaunchPad::filePrint()
{
    qWarning( "LaunchPad::filePrint(): Not implemented yet" );
}

void LaunchPad::fileExit()
{
    qWarning( "LaunchPad::fileExit(): Not implemented yet" );
}

