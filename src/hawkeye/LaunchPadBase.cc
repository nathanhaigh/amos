
#include "LaunchPadBase.hh"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <q3textedit.h>
#include <q3buttongroup.h>
#include <qradiobutton.h>
#include <q3header.h>
#include <q3listview.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <q3popupmenu.h>
#include <q3toolbar.h>
#include <qimage.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include <QtGui>

#include "icons/Hawkeye.xpm"

#include "NChartWidget.hh"
static const unsigned char image1_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xae, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xb5, 0x94, 0x51, 0x0e, 0xc3,
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
    0xcd, 0x8a, 0xb4, 0x79, 0x3c, 0xea, 0x0d, 0x05, 0x19, 0x70, 0xcd, 0xbc,
    0x4f, 0x01, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
    0x42, 0x60, 0x82
};


/*
 *  Constructs a LaunchPadBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
LaunchPadBase::LaunchPadBase( QWidget* parent, const char* name, Qt::WFlags fl )
    : Q3MainWindow( parent, name, fl )
{
    (void)statusBar();
    QImage img;
    img.loadFromData( image1_data, sizeof( image1_data ), "PNG" );
    image1 = img;
    if ( !name )
	setName( "LaunchPadBase" );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    LaunchPadBaseLayout = new Q3GridLayout( centralWidget(), 1, 1, 11, 6, "LaunchPadBaseLayout"); 

    tabWidget = new QTabWidget( centralWidget(), "tabWidget" );
    tabWidget->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, tabWidget->sizePolicy().hasHeightForWidth() ) );

    statsTab = new QWidget( tabWidget, "statsTab" );
    statsTabLayout = new Q3GridLayout( statsTab, 1, 1, 11, 6, "statsTabLayout"); 

    statsText = new Q3TextEdit( statsTab, "statsText" );
    statsText->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, statsText->sizePolicy().hasHeightForWidth() ) );
    statsText->setMinimumSize( QSize( 300, 0 ) );
    statsText->setTextFormat( Qt::RichText );
    statsText->setReadOnly( TRUE );

    statsTabLayout->addMultiCellWidget( statsText, 0, 2, 0, 0 );

    scaffoldSizes = new NChartWidget( statsTab, "scaffoldSizes" );

    statsTabLayout->addMultiCellWidget( scaffoldSizes, 0, 0, 1, 3 );

    contigSizes = new NChartWidget( statsTab, "contigSizes" );

    statsTabLayout->addMultiCellWidget( contigSizes, 1, 1, 1, 3 );

    orderRadioGroup = new Q3ButtonGroup( statsTab, "orderRadioGroup" );
    orderRadioGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, orderRadioGroup->sizePolicy().hasHeightForWidth() ) );
    orderRadioGroup->setColumnLayout(0, Qt::Vertical );
    orderRadioGroup->layout()->setSpacing( 6 );
    orderRadioGroup->layout()->setMargin( 11 );
    orderRadioGroupLayout = new Q3GridLayout( orderRadioGroup->layout() );
    orderRadioGroupLayout->setAlignment( Qt::AlignTop );

    radioButton8 = new QRadioButton( orderRadioGroup, "radioButton8" );
    radioButton8->setChecked( TRUE );

    orderRadioGroupLayout->addWidget( radioButton8, 0, 0 );

    radioButton9 = new QRadioButton( orderRadioGroup, "radioButton9" );

    orderRadioGroupLayout->addWidget( radioButton9, 0, 1 );

    radioButton10 = new QRadioButton( orderRadioGroup, "radioButton10" );

    orderRadioGroupLayout->addWidget( radioButton10, 0, 2 );

    statsTabLayout->addWidget( orderRadioGroup, 2, 1 );

    colorRadioGroup = new Q3ButtonGroup( statsTab, "colorRadioGroup" );
    colorRadioGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, colorRadioGroup->sizePolicy().hasHeightForWidth() ) );
    colorRadioGroup->setColumnLayout(0, Qt::Vertical );
    colorRadioGroup->layout()->setSpacing( 6 );
    colorRadioGroup->layout()->setMargin( 11 );
    colorRadioGroupLayout = new Q3GridLayout( colorRadioGroup->layout() );
    colorRadioGroupLayout->setAlignment( Qt::AlignTop );

    radioButton11 = new QRadioButton( colorRadioGroup, "radioButton11" );
    radioButton11->setChecked( TRUE );

    colorRadioGroupLayout->addWidget( radioButton11, 0, 0 );

    radioButton12 = new QRadioButton( colorRadioGroup, "radioButton12" );

    colorRadioGroupLayout->addWidget( radioButton12, 0, 1 );

    statsTabLayout->addWidget( colorRadioGroup, 2, 2 );
    spacer7 = new QSpacerItem( 81, 41, QSizePolicy::Expanding, QSizePolicy::Minimum );
    statsTabLayout->addItem( spacer7, 2, 3 );
    tabWidget->insertTab( statsTab, QString::fromLatin1("") );

    featuresTab = new QWidget( tabWidget, "featuresTab" );
    featuresTabLayout = new Q3GridLayout( featuresTab, 1, 1, 11, 6, "featuresTabLayout"); 

    featureList = new Q3ListView( featuresTab, "featureList" );
    featureList->setAllColumnsShowFocus( TRUE );
    featureList->setShowSortIndicator( TRUE );
    featureList->setRootIsDecorated( TRUE );

    featuresTabLayout->addMultiCellWidget( featureList, 0, 0, 0, 2 );

    groupGroup = new Q3ButtonGroup( featuresTab, "groupGroup" );
    groupGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, groupGroup->sizePolicy().hasHeightForWidth() ) );
    groupGroup->setColumnLayout(0, Qt::Vertical );
    groupGroup->layout()->setSpacing( 6 );
    groupGroup->layout()->setMargin( 11 );
    groupGroupLayout = new Q3HBoxLayout( groupGroup->layout() );
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
    librariesTabLayout = new Q3GridLayout( librariesTab, 1, 1, 11, 6, "librariesTabLayout"); 

    libraryList = new Q3ListView( librariesTab, "libraryList" );
    libraryList->setAllColumnsShowFocus( TRUE );
    libraryList->setShowSortIndicator( TRUE );
    libraryList->setRootIsDecorated( TRUE );

    librariesTabLayout->addWidget( libraryList, 0, 0 );

    histogramGroup = new Q3ButtonGroup( librariesTab, "histogramGroup" );
    histogramGroup->setColumnLayout(0, Qt::Vertical );
    histogramGroup->layout()->setSpacing( 6 );
    histogramGroup->layout()->setMargin( 11 );
    histogramGroupLayout = new Q3GridLayout( histogramGroup->layout() );
    histogramGroupLayout->setAlignment( Qt::AlignTop );

    libraryClearLengthButton = new QPushButton( histogramGroup, "libraryClearLengthButton" );
    libraryClearLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, libraryClearLengthButton->sizePolicy().hasHeightForWidth() ) );

    histogramGroupLayout->addWidget( libraryClearLengthButton, 0, 1 );

    libraryInsertButton = new QPushButton( histogramGroup, "libraryInsertButton" );
    libraryInsertButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, libraryInsertButton->sizePolicy().hasHeightForWidth() ) );

    histogramGroupLayout->addWidget( libraryInsertButton, 0, 0 );

    libraryReadLengthButton = new QPushButton( histogramGroup, "libraryReadLengthButton" );
    libraryReadLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, libraryReadLengthButton->sizePolicy().hasHeightForWidth() ) );

    histogramGroupLayout->addWidget( libraryReadLengthButton, 0, 2 );

    libraryGCButton = new QPushButton( histogramGroup, "libraryGCButton" );
    libraryGCButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, libraryGCButton->sizePolicy().hasHeightForWidth() ) );

    histogramGroupLayout->addWidget( libraryGCButton, 0, 3 );

    librariesTabLayout->addWidget( histogramGroup, 1, 0 );
    tabWidget->insertTab( librariesTab, QString::fromLatin1("") );

    scaffoldsTab = new QWidget( tabWidget, "scaffoldsTab" );
    scaffoldsTabLayout = new Q3GridLayout( scaffoldsTab, 1, 1, 11, 6, "scaffoldsTabLayout"); 

    scaffoldList = new Q3ListView( scaffoldsTab, "scaffoldList" );
    scaffoldList->setAllColumnsShowFocus( TRUE );
    scaffoldList->setShowSortIndicator( TRUE );
    scaffoldList->setRootIsDecorated( TRUE );

    scaffoldsTabLayout->addMultiCellWidget( scaffoldList, 0, 0, 0, 3 );

    textLabel1 = new QLabel( scaffoldsTab, "textLabel1" );
    textLabel1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

    scaffoldsTabLayout->addWidget( textLabel1, 1, 0 );

    textLabel1_2 = new QLabel( scaffoldsTab, "textLabel1_2" );
    textLabel1_2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

    scaffoldsTabLayout->addWidget( textLabel1_2, 2, 0 );

    scaffoldIIDEdit = new QLineEdit( scaffoldsTab, "scaffoldIIDEdit" );
    scaffoldIIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, scaffoldIIDEdit->sizePolicy().hasHeightForWidth() ) );

    scaffoldsTabLayout->addWidget( scaffoldIIDEdit, 1, 1 );

    scaffoldEIDEdit = new QLineEdit( scaffoldsTab, "scaffoldEIDEdit" );
    scaffoldEIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, scaffoldEIDEdit->sizePolicy().hasHeightForWidth() ) );

    scaffoldsTabLayout->addWidget( scaffoldEIDEdit, 2, 1 );

    scaffoldHistogramGroup = new Q3ButtonGroup( scaffoldsTab, "scaffoldHistogramGroup" );
    scaffoldHistogramGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, scaffoldHistogramGroup->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup->setColumnLayout(0, Qt::Vertical );
    scaffoldHistogramGroup->layout()->setSpacing( 6 );
    scaffoldHistogramGroup->layout()->setMargin( 11 );
    scaffoldHistogramGroupLayout = new Q3HBoxLayout( scaffoldHistogramGroup->layout() );
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
    contigsTabLayout = new Q3GridLayout( contigsTab, 1, 1, 11, 6, "contigsTabLayout"); 

    textLabel1_2_2 = new QLabel( contigsTab, "textLabel1_2_2" );
    textLabel1_2_2->setAlignment( Qt::AlignVCenter | Qt::AlignRight  );

    contigsTabLayout->addWidget( textLabel1_2_2, 2, 0 );

    textLabel1_3 = new QLabel( contigsTab, "textLabel1_3" );
    textLabel1_3->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

    contigsTabLayout->addWidget( textLabel1_3, 1, 0 );

    scaffoldHistogramGroup_2 = new Q3ButtonGroup( contigsTab, "scaffoldHistogramGroup_2" );
    scaffoldHistogramGroup_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, scaffoldHistogramGroup_2->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2->setColumnLayout(0, Qt::Vertical );
    scaffoldHistogramGroup_2->layout()->setSpacing( 6 );
    scaffoldHistogramGroup_2->layout()->setMargin( 11 );
    scaffoldHistogramGroup_2Layout = new Q3HBoxLayout( scaffoldHistogramGroup_2->layout() );
    scaffoldHistogramGroup_2Layout->setAlignment( Qt::AlignTop );

    contigLengthButton = new QPushButton( scaffoldHistogramGroup_2, "contigLengthButton" );
    contigLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, contigLengthButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigLengthButton );

    contigReadsButton = new QPushButton( scaffoldHistogramGroup_2, "contigReadsButton" );
    contigReadsButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, contigReadsButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigReadsButton );

    contigCoverageButton = new QPushButton( scaffoldHistogramGroup_2, "contigCoverageButton" );
    contigCoverageButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, contigCoverageButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigCoverageButton );

    contigGCButton = new QPushButton( scaffoldHistogramGroup_2, "contigGCButton" );
    contigGCButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, contigGCButton->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2Layout->addWidget( contigGCButton );

    contigsTabLayout->addMultiCellWidget( scaffoldHistogramGroup_2, 1, 2, 3, 3 );

    contigList = new Q3ListView( contigsTab, "contigList" );
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

    buttonGroup7 = new Q3ButtonGroup( contigsTab, "buttonGroup7" );
    buttonGroup7->setColumnLayout(0, Qt::Vertical );
    buttonGroup7->layout()->setSpacing( 6 );
    buttonGroup7->layout()->setMargin( 11 );
    buttonGroup7Layout = new Q3VBoxLayout( buttonGroup7->layout() );
    buttonGroup7Layout->setAlignment( Qt::AlignTop );

    readsCheck = new QCheckBox( buttonGroup7, "readsCheck" );
    buttonGroup7Layout->addWidget( readsCheck );

    contigsTabLayout->addMultiCellWidget( buttonGroup7, 1, 2, 2, 2 );
    tabWidget->insertTab( contigsTab, QString::fromLatin1("") );

    TabPage = new QWidget( tabWidget, "TabPage" );
    TabPageLayout = new Q3GridLayout( TabPage, 1, 1, 11, 6, "TabPageLayout"); 

    textLabel1_3_2 = new QLabel( TabPage, "textLabel1_3_2" );
    textLabel1_3_2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

    TabPageLayout->addWidget( textLabel1_3_2, 1, 0 );

    readEIDEdit = new QLineEdit( TabPage, "readEIDEdit" );
    readEIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, readEIDEdit->sizePolicy().hasHeightForWidth() ) );

    TabPageLayout->addWidget( readEIDEdit, 2, 1 );

    textLabel1_2_2_2 = new QLabel( TabPage, "textLabel1_2_2_2" );
    textLabel1_2_2_2->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

    TabPageLayout->addWidget( textLabel1_2_2_2, 2, 0 );

    readIIDEdit = new QLineEdit( TabPage, "readIIDEdit" );
    readIIDEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, readIIDEdit->sizePolicy().hasHeightForWidth() ) );

    TabPageLayout->addWidget( readIIDEdit, 1, 1 );

    scaffoldHistogramGroup_2_2 = new Q3ButtonGroup( TabPage, "scaffoldHistogramGroup_2_2" );
    scaffoldHistogramGroup_2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, scaffoldHistogramGroup_2_2->sizePolicy().hasHeightForWidth() ) );
    scaffoldHistogramGroup_2_2->setColumnLayout(0, Qt::Vertical );
    scaffoldHistogramGroup_2_2->layout()->setSpacing( 6 );
    scaffoldHistogramGroup_2_2->layout()->setMargin( 11 );
    scaffoldHistogramGroup_2_2Layout = new Q3GridLayout( scaffoldHistogramGroup_2_2->layout() );
    scaffoldHistogramGroup_2_2Layout->setAlignment( Qt::AlignTop );

    readLengthButton = new QPushButton( scaffoldHistogramGroup_2_2, "readLengthButton" );
    readLengthButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, readLengthButton->sizePolicy().hasHeightForWidth() ) );

    scaffoldHistogramGroup_2_2Layout->addWidget( readLengthButton, 0, 0 );

    readGCButton = new QPushButton( scaffoldHistogramGroup_2_2, "readGCButton" );
    readGCButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, readGCButton->sizePolicy().hasHeightForWidth() ) );

    scaffoldHistogramGroup_2_2Layout->addWidget( readGCButton, 0, 1 );

    TabPageLayout->addMultiCellWidget( scaffoldHistogramGroup_2_2, 1, 2, 2, 2 );

    readList = new Q3ListView( TabPage, "readList" );
    readList->setAllColumnsShowFocus( TRUE );
    readList->setShowSortIndicator( TRUE );
    readList->setRootIsDecorated( TRUE );

    TabPageLayout->addMultiCellWidget( readList, 0, 0, 0, 3 );
    tabWidget->insertTab( TabPage, QString::fromLatin1("") );

    LaunchPadBaseLayout->addWidget( tabWidget, 0, 0 );

    // actions
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIcon( image1 ) );
    fileQuitAction = new QAction( this, "fileQuitAction" );
    fileChromatogramPathsAction = new QAction( this, "fileChromatogramPathsAction" );

    fileImportAction = new QAction( this, "fileImportAction" );
    loadAmosFileAction = new QAction( this, "loadAmosFileAction" );
    loadKmersAction = new QAction(this, "loadKmersAction");


    // toolbars
    Toolbar = new Q3ToolBar( QString(""), this, Qt::DockRight ); 

    QLabel * logo = new QLabel(Toolbar, "logolabel");
    QPixmap pixmap((const char **)Hawkeye_xpm);
        
    logo->setPixmap(pixmap.scaledToWidth(150, Qt::SmoothTransformation));


    contigIDLabel = new QLabel( Toolbar, "contigIDLabel" );

    contigIDSpin = new QSpinBox( Toolbar, "contigIDSpin" );
    contigIDSpin->setMinimumSize( QSize(100,25));
    Toolbar->addSeparator();

    viewButton = new QPushButton( Toolbar, "viewButton" );
    viewButton->setMinimumSize( QSize( 0, 25 ) );
    Toolbar->addSeparator();

    insertButton = new QPushButton( Toolbar, "insertButton" );
    insertButton->setMinimumSize( QSize( 0, 25 ) );
    Toolbar->addSeparator();

    tilingButton = new QPushButton( Toolbar, "tilingButton" );
    tilingButton->setMinimumSize( QSize( 0, 25 ) );


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );


    fileMenu = new Q3PopupMenu( this );
    fileOpenAction->addTo( fileMenu );
    fileImportAction->addTo( fileMenu );
    loadAmosFileAction->addTo( fileMenu );
    loadKmersAction->addTo(fileMenu);
    fileChromatogramPathsAction->addTo( fileMenu );
    
    fileMenu->insertSeparator();
    fileQuitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 1 );

    languageChange();
    resize( QSize(937, 738).expandedTo(minimumSizeHint()) );
    //clearWindowState( Qt::WState_Polished );

    // signals and slots connections
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileQuitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( fileChromatogramPathsAction, SIGNAL( activated() ), this, SLOT( fileChromoPaths() ) );
    connect( fileImportAction, SIGNAL( activated() ), this, SLOT( fileImport() ) );
    connect( loadAmosFileAction, SIGNAL( activated()), this, SLOT( loadAmosFile()));
    connect( loadKmersAction, SIGNAL(activated()), this, SLOT(loadKmersFile()));

    connect( new QShortcut(QKeySequence(tr("Ctrl+Q")), this), SIGNAL(activated()),
             qApp, SLOT(quit()));
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
    setCaption( tr( "Hawkeye" ) );
    orderRadioGroup->setTitle( tr( "Ordering" ) );
    radioButton8->setText( tr( "Size" ) );
    QToolTip::add( radioButton8, tr( "Sort Objects by Size" ) );
    radioButton9->setText( tr( "Feature Density" ) );
    QToolTip::add( radioButton9, tr( "Sort Objects By Feature Density" ) );
    radioButton10->setText( tr( "Treemap" ) );
    QToolTip::add( radioButton10, tr( "Display Objects in a Treemap" ) );
    colorRadioGroup->setTitle( tr( "Coloring" ) );
    radioButton11->setText( tr( "Green - Red" ) );
    QToolTip::add( radioButton11, tr( "Color objects with a green to red gradient" ) );
    radioButton12->setText( tr( "White - Red" ) );
    QToolTip::add( radioButton12, tr( "Color objects with a white to red gradient" ) );
    tabWidget->changeTab( statsTab, tr( "Statistics" ) );
    groupGroup->setTitle( tr( "Group by" ) );
    featureGroupContigButton->setText( tr( "Contig" ) );
    featureGroupTypeButton->setText( tr( "Type" ) );
    tabWidget->changeTab( featuresTab, tr( "Features" ) );
    histogramGroup->setTitle( tr( "Distributions" ) );
    libraryClearLengthButton->setText( tr( "Read Clear Range" ) );
    QToolTip::add( libraryClearLengthButton, tr( "Display histogram of the clear range length for all reads in the selected library" ) );
    libraryInsertButton->setText( tr( "Insert Size" ) );
    QToolTip::add( libraryInsertButton, tr( "Display histogram of inserts for selected library" ) );
    libraryReadLengthButton->setText( tr( "Read Length" ) );
    QToolTip::add( libraryReadLengthButton, tr( "Display a histogram of the full read length for all reads in the library" ) );
    libraryGCButton->setText( tr( "GC Content" ) );
    QToolTip::add( libraryGCButton, tr( "Display a histogram of the GC content for all the reads in the library" ) );
    tabWidget->changeTab( librariesTab, tr( "Libraries" ) );
    textLabel1->setText( tr( "IID:" ) );
    textLabel1_2->setText( tr( "EID:" ) );
    QToolTip::add( scaffoldIIDEdit, tr( "Search for a scaffold or contig by IID" ) );
    QToolTip::add( scaffoldEIDEdit, tr( "Search for a scaffold or contig by EID" ) );
    scaffoldHistogramGroup->setTitle( tr( "Distributions" ) );
    scaffoldSpanButton->setText( tr( "Span" ) );
    QToolTip::add( scaffoldSpanButton, tr( "Display the distribution of scaffold sizes" ) );
    scaffoldContigsButton->setText( tr( "Contig Count" ) );
    QToolTip::add( scaffoldContigsButton, tr( "Display a histogram of the number of contigs in each scaffold" ) );
    tabWidget->changeTab( scaffoldsTab, tr( "Scaffolds" ) );
    textLabel1_2_2->setText( tr( "EID:" ) );
    textLabel1_3->setText( tr( "IID:" ) );
    scaffoldHistogramGroup_2->setTitle( tr( "Distributions" ) );
    contigLengthButton->setText( tr( "Length" ) );
    QToolTip::add( contigLengthButton, tr( "Display the distribution of the length of all contigs" ) );
    contigReadsButton->setText( tr( "Read Count" ) );
    QToolTip::add( contigReadsButton, tr( "Display a histogram of the number of reads in each contig" ) );
    contigCoverageButton->setText(tr("Fold Coverage"));
    QToolTip::add( contigCoverageButton, tr( "Display a histogram of each contig's read fold coverage"));
    contigGCButton->setText( tr( "GC Content" ) );
    QToolTip::add( contigGCButton, tr( "Display a histogram of the GC content of each contig" ) );
    QToolTip::add( contigEIDEdit, tr( "Search for a contig (or read) by EID" ) );
    QToolTip::add( contigIIDEdit, tr( "Search for a contig (or read) by IID" ) );
    buttonGroup7->setTitle( tr( "Display" ) );
    readsCheck->setText( tr( "Reads" ) );
    QToolTip::add( readsCheck, tr( "Include the reads in the table (May take a long time to load)" ) );
    tabWidget->changeTab( contigsTab, tr( "Contigs" ) );
    textLabel1_3_2->setText( tr( "IID:" ) );
    QToolTip::add( readEIDEdit, tr( "Search for a read by EID" ) );
    textLabel1_2_2_2->setText( tr( "EID:" ) );
    QToolTip::add( readIIDEdit, tr( "Search for a read by IID" ) );
    scaffoldHistogramGroup_2_2->setTitle( tr( "Distributions" ) );
    readLengthButton->setText( tr( "Length" ) );
    QToolTip::add( readLengthButton, tr( "Display a histogram of the full read length" ) );
    readGCButton->setText( tr( "GC Content" ) );
    QToolTip::add( readGCButton, tr( "Display a histogram of the GC content of these reads" ) );
    tabWidget->changeTab( TabPage, tr( "Reads" ) );

    fileOpenAction->setText( tr( "Open Bank" ) );
    fileOpenAction->setMenuText( tr( "&Open Bank..." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );

    fileQuitAction->setText( tr( "Quit" ) );
    fileQuitAction->setMenuText( tr( "&Quit" ) );
    fileQuitAction->setAccel( tr( "Ctrl+Q" ) ); 

    loadAmosFileAction->setText(tr("Import AFG..."));
    loadAmosFileAction->setMenuText(tr("&Import AFG..."));
    loadAmosFileAction->setAccel(tr("Ctrl+L"));

    fileImportAction->setText( tr( "Import ACE..." ) );
    fileImportAction->setMenuText( tr( "&Import ACE..." ) );
    fileImportAction->setAccel( tr( "Ctrl+I" ) );

    loadKmersAction->setText(tr("Open Kmers..."));
    loadKmersAction->setMenuText(tr("Open &Kmers..."));
    loadKmersAction->setAccel(tr("Ctrl+K"));

    fileChromatogramPathsAction->setText( tr( "Set Chromatogram Paths" ) );
    fileChromatogramPathsAction->setMenuText( tr( "Set Chromatogram &Paths..." ) );
    fileChromatogramPathsAction->setAccel( tr( "Ctrl+P" ) );

    Toolbar->setLabel( tr( "Toolbar" ) );
    contigIDLabel->setText( tr( "Contig ID" ) );
    viewButton->setText( tr( "All Views" ) );
    QToolTip::add( viewButton, tr( "Display All Views" ) );
    insertButton->setText( tr( "Scaffold View" ) );
    QToolTip::add( insertButton, tr( "Display scaffold of current contig" ) );
    tilingButton->setText( tr( "Contig View" ) );
    QToolTip::add( tilingButton, tr( "Display read tiling of current contig" ) );
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

void LaunchPadBase::loadAmosFile()
{
    qWarning( "LaunchPadBase::loadAmosFile(): Not implemented yet" );
}

void LaunchPadBase::loadKmersFile()
{
    qWarning( "LaunchPadBase::loadKmersFile(): Not implemented yet" );
}

void LaunchPadBase::fileImport()
{
    qWarning( "LaunchPadBase::fileImport(): Not implemented yet" );
}

