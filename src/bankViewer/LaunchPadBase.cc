/****************************************************************************
** Form implementation generated from reading ui file 'LaunchPadBase.ui'
**
** Created: Thu Apr 27 23:37:26 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "LaunchPadBase.hh"

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
#include <qcheckbox.h>
#include <qspinbox.h>
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
    0xae, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xb5, 0x94, 0x51, 0x0e, 0xc3,
    0x20, 0x0c, 0x43, 0x6d, 0xb4, 0x53, 0x71, 0xb6, 0x69, 0x1f, 0xd3, 0xce,
    0xe6, 0x6b, 0xb1, 0x8f, 0x51, 0x35, 0xa2, 0x19, 0x23, 0x5a, 0xf0, 0x4f,
    0x44, 0x2b, 0x5e, 0x2c, 0x03, 0xa1, 0x24, 0xec, 0x50, 0xd9, 0x42, 0xdd,
    0x09, 0xbe, 0x45, 0x37, 0xd4, 0x5a, 0x9b, 0x5d, 0x4b, 0xe2, 0xf8, 0x5f,
    0x12, 0x43, 0xe0, 0x63, 0x93, 0xd7, 0x48, 0x12, 0x6d, 0x53, 0x66, 0x1d,
    0x9e, 0x85, 0x4a, 0x62, 0x4a, 0xc6, 0x63, 0x3c, 0x40, 0xcf, 0xf8, 0x57,
    0x6e, 0x51, 0x28, 0x00, 0x10, 0x40, 0x6b, 0x0d, 0x00, 0xda, 0xb1, 0x04,
    0xb9, 0xc6, 0x9d, 0x19, 0xe8, 0x87, 0x77, 0x42, 0x01, 0xf6, 0x4e, 0xbc,
    0x7c, 0x8f, 0x34, 0x2f, 0xc6, 0x78, 0xb0, 0xce, 0x55, 0x3c, 0xc7, 0x6b,
    0x75, 0x2e, 0x37, 0xe3, 0x68, 0xd6, 0x9e, 0xdc, 0x8c, 0x2d, 0xf4, 0xf5,
    0x8c, 0x01, 0xef, 0x8f, 0x4f, 0x75, 0x33, 0xfe, 0x17, 0x6a, 0x1e, 0x48,
    0x2e, 0xf4, 0xe2, 0x38, 0x0b, 0x6a, 0xc0, 0xb9, 0xd0, 0xd3, 0x6a, 0x57,
    0x16, 0x14, 0x30, 0x83, 0x3e, 0x13, 0x0a, 0xac, 0xde, 0xf6, 0x2f, 0x9a,
    0xcd, 0x8a, 0xb4, 0x79, 0x3c, 0xea, 0x0d, 0x05, 0x19, 0x70, 0xcd, 0x5b,
    0x32, 0xcc, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
    0x42, 0x60, 0x82
};


/*
 *  Constructs a LaunchPadBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
LaunchPadBase::LaunchPadBase( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    QImage img;
    img.loadFromData( image0_data, sizeof( image0_data ), "PNG" );
    image0 = img;
    if ( !name )
	setName( "LaunchPadBase" );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    LaunchPadBaseLayout = new QHBoxLayout( centralWidget(), 11, 6, "LaunchPadBaseLayout"); 

    tabWidget = new QTabWidget( centralWidget(), "tabWidget" );
    tabWidget->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, tabWidget->sizePolicy().hasHeightForWidth() ) );

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

    featureGroupContigButton = new QRadioButton( groupGroup, "featureGroupContigButton" );
    featureGroupContigButton->setChecked( TRUE );
    groupGroupLayout->addWidget( featureGroupContigButton );

    featureGroupTypeButton = new QRadioButton( groupGroup, "featureGroupTypeButton" );
    featureGroupTypeButton->setChecked( FALSE );
    groupGroupLayout->addWidget( featureGroupTypeButton );

    featuresTabLayout->addWidget( groupGroup, 1, 0 );
    spacer1 = new QSpacerItem( 190, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    featuresTabLayout->addItem( spacer1, 1, 1 );
    tabWidget->insertTab( featuresTab, QString::fromLatin1("") );

    librariesTab = new QWidget( tabWidget, "librariesTab" );
    librariesTabLayout = new QGridLayout( librariesTab, 1, 1, 11, 6, "librariesTabLayout"); 

    libraryList = new QListView( librariesTab, "libraryList" );
    libraryList->setAllColumnsShowFocus( TRUE );
    libraryList->setShowSortIndicator( TRUE );
    libraryList->setRootIsDecorated( TRUE );

    librariesTabLayout->addMultiCellWidget( libraryList, 0, 0, 0, 1 );

    histogramGroup = new QButtonGroup( librariesTab, "histogramGroup" );
    histogramGroup->setColumnLayout(0, Qt::Vertical );
    histogramGroup->layout()->setSpacing( 6 );
    histogramGroup->layout()->setMargin( 11 );
    histogramGroupLayout = new QHBoxLayout( histogramGroup->layout() );
    histogramGroupLayout->setAlignment( Qt::AlignTop );

    libraryInsertButton = new QPushButton( histogramGroup, "libraryInsertButton" );
    libraryInsertButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, libraryInsertButton->sizePolicy().hasHeightForWidth() ) );
    histogramGroupLayout->addWidget( libraryInsertButton );

    libraryClearLengthButton = new QPushButton( histogramGroup, "libraryClearLengthButton" );
    libraryClearLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, libraryClearLengthButton->sizePolicy().hasHeightForWidth() ) );
    histogramGroupLayout->addWidget( libraryClearLengthButton );

    libraryReadLengthButton = new QPushButton( histogramGroup, "libraryReadLengthButton" );
    libraryReadLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, libraryReadLengthButton->sizePolicy().hasHeightForWidth() ) );
    histogramGroupLayout->addWidget( libraryReadLengthButton );

    libraryGCButton = new QPushButton( histogramGroup, "libraryGCButton" );
    libraryGCButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, libraryGCButton->sizePolicy().hasHeightForWidth() ) );
    histogramGroupLayout->addWidget( libraryGCButton );

    librariesTabLayout->addWidget( histogramGroup, 1, 1 );
    spacer2 = new QSpacerItem( 161, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    librariesTabLayout->addItem( spacer2, 1, 0 );
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
    scaffoldSpanButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, scaffoldSpanButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroupLayout->addWidget( scaffoldSpanButton );

    scaffoldContigsButton = new QPushButton( scaffoldHistogramGroup, "scaffoldContigsButton" );
    scaffoldContigsButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, scaffoldContigsButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroupLayout->addWidget( scaffoldContigsButton );

    scaffoldsTabLayout->addMultiCellWidget( scaffoldHistogramGroup, 1, 2, 2, 2 );
    tabWidget->insertTab( scaffoldsTab, QString::fromLatin1("") );

    contigsTab = new QWidget( tabWidget, "contigsTab" );
    contigsTabLayout = new QGridLayout( contigsTab, 1, 1, 11, 6, "contigsTabLayout"); 

    textLabel1_2_2 = new QLabel( contigsTab, "textLabel1_2_2" );
    textLabel1_2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    contigsTabLayout->addWidget( textLabel1_2_2, 2, 0 );

    textLabel1_3 = new QLabel( contigsTab, "textLabel1_3" );
    textLabel1_3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    contigsTabLayout->addWidget( textLabel1_3, 1, 0 );

    scaffoldHistogramGroup_2 = new QButtonGroup( contigsTab, "scaffoldHistogramGroup_2" );
    scaffoldHistogramGroup_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, scaffoldHistogramGroup_2->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2->setColumnLayout(0, Qt::Vertical );
    scaffoldHistogramGroup_2->layout()->setSpacing( 6 );
    scaffoldHistogramGroup_2->layout()->setMargin( 11 );
    scaffoldHistogramGroup_2Layout = new QHBoxLayout( scaffoldHistogramGroup_2->layout() );
    scaffoldHistogramGroup_2Layout->setAlignment( Qt::AlignTop );

    contigLengthButton = new QPushButton( scaffoldHistogramGroup_2, "contigLengthButton" );
    contigLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, contigLengthButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigLengthButton );

    contigReadsButton = new QPushButton( scaffoldHistogramGroup_2, "contigReadsButton" );
    contigReadsButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, contigReadsButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigReadsButton );

    contigGCButton = new QPushButton( scaffoldHistogramGroup_2, "contigGCButton" );
    contigGCButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, contigGCButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigGCButton );

    contigsTabLayout->addMultiCellWidget( scaffoldHistogramGroup_2, 1, 2, 3, 3 );

    contigList = new QListView( contigsTab, "contigList" );
    contigList->setAllColumnsShowFocus( TRUE );
    contigList->setShowSortIndicator( TRUE );
    contigList->setRootIsDecorated( TRUE );

    contigsTabLayout->addMultiCellWidget( contigList, 0, 0, 0, 3 );

    contigEIDEdit = new QLineEdit( contigsTab, "contigEIDEdit" );
    contigEIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, contigEIDEdit->sizePolicy().hasHeightForWidth() ) );

    contigsTabLayout->addWidget( contigEIDEdit, 2, 1 );

    contigIIDEdit = new QLineEdit( contigsTab, "contigIIDEdit" );
    contigIIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, contigIIDEdit->sizePolicy().hasHeightForWidth() ) );

    contigsTabLayout->addWidget( contigIIDEdit, 1, 1 );

    buttonGroup7 = new QButtonGroup( contigsTab, "buttonGroup7" );
    buttonGroup7->setColumnLayout(0, Qt::Vertical );
    buttonGroup7->layout()->setSpacing( 6 );
    buttonGroup7->layout()->setMargin( 11 );
    buttonGroup7Layout = new QVBoxLayout( buttonGroup7->layout() );
    buttonGroup7Layout->setAlignment( Qt::AlignTop );

    readsCheck = new QCheckBox( buttonGroup7, "readsCheck" );
    buttonGroup7Layout->addWidget( readsCheck );

    contigsTabLayout->addMultiCellWidget( buttonGroup7, 1, 2, 2, 2 );
    tabWidget->insertTab( contigsTab, QString::fromLatin1("") );

    TabPage = new QWidget( tabWidget, "TabPage" );
    TabPageLayout = new QGridLayout( TabPage, 1, 1, 11, 6, "TabPageLayout"); 

    textLabel1_3_2 = new QLabel( TabPage, "textLabel1_3_2" );
    textLabel1_3_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    TabPageLayout->addWidget( textLabel1_3_2, 1, 0 );

    readEIDEdit = new QLineEdit( TabPage, "readEIDEdit" );
    readEIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, readEIDEdit->sizePolicy().hasHeightForWidth() ) );

    TabPageLayout->addWidget( readEIDEdit, 2, 1 );

    textLabel1_2_2_2 = new QLabel( TabPage, "textLabel1_2_2_2" );
    textLabel1_2_2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    TabPageLayout->addWidget( textLabel1_2_2_2, 2, 0 );

    readIIDEdit = new QLineEdit( TabPage, "readIIDEdit" );
    readIIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, readIIDEdit->sizePolicy().hasHeightForWidth() ) );

    TabPageLayout->addWidget( readIIDEdit, 1, 1 );

    scaffoldHistogramGroup_2_2 = new QButtonGroup( TabPage, "scaffoldHistogramGroup_2_2" );
    scaffoldHistogramGroup_2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, scaffoldHistogramGroup_2_2->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2_2->setColumnLayout(0, Qt::Vertical );
    scaffoldHistogramGroup_2_2->layout()->setSpacing( 6 );
    scaffoldHistogramGroup_2_2->layout()->setMargin( 11 );
    scaffoldHistogramGroup_2_2Layout = new QGridLayout( scaffoldHistogramGroup_2_2->layout() );
    scaffoldHistogramGroup_2_2Layout->setAlignment( Qt::AlignTop );

    readLengthButton = new QPushButton( scaffoldHistogramGroup_2_2, "readLengthButton" );
    readLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, readLengthButton->sizePolicy().hasHeightForWidth() ) );

    scaffoldHistogramGroup_2_2Layout->addWidget( readLengthButton, 0, 0 );

    readGCButton = new QPushButton( scaffoldHistogramGroup_2_2, "readGCButton" );
    readGCButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, readGCButton->sizePolicy().hasHeightForWidth() ) );

    scaffoldHistogramGroup_2_2Layout->addWidget( readGCButton, 0, 1 );

    TabPageLayout->addMultiCellWidget( scaffoldHistogramGroup_2_2, 1, 2, 2, 2 );

    readList = new QListView( TabPage, "readList" );
    readList->setAllColumnsShowFocus( TRUE );
    readList->setShowSortIndicator( TRUE );
    readList->setRootIsDecorated( TRUE );

    TabPageLayout->addMultiCellWidget( readList, 0, 0, 0, 3 );
    tabWidget->insertTab( TabPage, QString::fromLatin1("") );
    LaunchPadBaseLayout->addWidget( tabWidget );

    // actions
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( image0 ) );
    fileQuitAction = new QAction( this, "fileQuitAction" );
    fileChromatogramPathsAction = new QAction( this, "fileChromatogramPathsAction" );


    // toolbars
    Toolbar = new QToolBar( QString(""), this, DockTop ); 


    contigIDLabel = new QLabel( Toolbar, "contigIDLabel" );

    contigIDSpin = new QSpinBox( Toolbar, "contigIDSpin" );
    Toolbar->addSeparator();

    viewButton = new QPushButton( Toolbar, "viewButton" );
    viewButton->setMinimumSize( QSize( 0, 25 ) );
    Toolbar->addSeparator();

    insertButton = new QPushButton( Toolbar, "insertButton" );
    insertButton->setMinimumSize( QSize( 0, 25 ) );
    insertButton->setMaximumSize( QSize( 32767, 20 ) );
    Toolbar->addSeparator();

    tilingButton = new QPushButton( Toolbar, "tilingButton" );
    tilingButton->setMinimumSize( QSize( 0, 25 ) );


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );


    fileMenu = new QPopupMenu( this );
    fileOpenAction->addTo( fileMenu );
    fileChromatogramPathsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileQuitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 1 );

    languageChange();
    resize( QSize(666, 789).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileQuitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( fileChromatogramPathsAction, SIGNAL( activated() ), this, SLOT( fileChromoPaths() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
LaunchPadBase::~LaunchPadBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void LaunchPadBase::languageChange()
{
    setCaption( tr( "Assembly Investigator" ) );
    tabWidget->changeTab( statsTab, tr( "Statistics" ) );
    groupGroup->setTitle( tr( "Group by" ) );
    featureGroupContigButton->setText( tr( "Contig" ) );
    featureGroupTypeButton->setText( tr( "Type" ) );
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
    tabWidget->changeTab( scaffoldsTab, tr( "Scaffolds" ) );
    textLabel1_2_2->setText( tr( "EID:" ) );
    textLabel1_3->setText( tr( "IID:" ) );
    scaffoldHistogramGroup_2->setTitle( tr( "Histograms" ) );
    contigLengthButton->setText( tr( "Length" ) );
    contigReadsButton->setText( tr( "Read Count" ) );
    contigGCButton->setText( tr( "GC Content" ) );
    buttonGroup7->setTitle( tr( "Display" ) );
    readsCheck->setText( tr( "Reads" ) );
    tabWidget->changeTab( contigsTab, tr( "Contigs" ) );
    textLabel1_3_2->setText( tr( "IID:" ) );
    textLabel1_2_2_2->setText( tr( "EID:" ) );
    scaffoldHistogramGroup_2_2->setTitle( tr( "Histograms" ) );
    readLengthButton->setText( tr( "Length" ) );
    readGCButton->setText( tr( "GC Content" ) );
    tabWidget->changeTab( TabPage, tr( "Reads" ) );
    fileOpenAction->setText( tr( "Open" ) );
    fileOpenAction->setMenuText( tr( "&Open..." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );
    fileQuitAction->setText( tr( "Quit" ) );
    fileQuitAction->setMenuText( tr( "&Quit" ) );
    fileQuitAction->setAccel( tr( "Ctrl+Q" ) );
    fileChromatogramPathsAction->setText( tr( "Chromatogram Paths" ) );
    fileChromatogramPathsAction->setMenuText( tr( "Chromatogram &Paths..." ) );
    fileChromatogramPathsAction->setAccel( tr( "Ctrl+P" ) );
    Toolbar->setLabel( tr( "Toolbar" ) );
    contigIDLabel->setText( tr( "Contig ID" ) );
    viewButton->setText( tr( "All Views" ) );
    insertButton->setText( tr( "Insert View" ) );
    tilingButton->setText( tr( "Tiling View" ) );
    if (MenuBar->findItem(1))
        MenuBar->findItem(1)->setText( tr( "&File" ) );
}

void LaunchPadBase::fileNew()
{
    qWarning( "LaunchPadBase::fileNew(): Not implemented yet" );
}

void LaunchPadBase::fileOpen()
{
    qWarning( "LaunchPadBase::fileOpen(): Not implemented yet" );
}

void LaunchPadBase::fileSave()
{
    qWarning( "LaunchPadBase::fileSave(): Not implemented yet" );
}

void LaunchPadBase::fileSaveAs()
{
    qWarning( "LaunchPadBase::fileSaveAs(): Not implemented yet" );
}

void LaunchPadBase::filePrint()
{
    qWarning( "LaunchPadBase::filePrint(): Not implemented yet" );
}

void LaunchPadBase::fileExit()
{
    qWarning( "LaunchPadBase::fileExit(): Not implemented yet" );
}

void LaunchPadBase::fileChromoPaths()
{
    qWarning( "LaunchPadBase::fileChromoPaths(): Not implemented yet" );
}

