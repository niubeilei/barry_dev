/****************************************************************************
** Form implementation generated from reading ui file 'MainWindow.ui'
**
** Created: Thu Feb 21 03:35:55 2008
**      by: The User Interface Compiler ($Id: MainWindow.cpp,v 1.2 2015/01/06 08:57:50 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "MainWindow.h"

#include <qvariant.h>
#include <qworkspace.h>
#include <qheader.h>
#include <qmessagebox.h>
#include <qvbox.h>
#include <qstatusbar.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "MDIWindow.h"
#include "MDIWndRVG.h"
#include "NewRVGDialog.h"
#include "RVGTestDlg.h"
#include "../MainWindow.ui.h"
/*
 *  Constructs a MainWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
MainWindow::MainWindow( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "MainWindow" );

    // actions
    RVGNewAction = new QAction( this, "RVGNewAction" );
    RVGNewAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filenew" ) ) );
    SaveAction = new QAction( this, "SaveAction" );
    SaveAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );
    helpContentsAction = new QAction( this, "helpContentsAction" );
    helpIndexAction = new QAction( this, "helpIndexAction" );
    helpAboutAction = new QAction( this, "helpAboutAction" );
    mainOpen_projectAction = new QAction( this, "mainOpen_projectAction" );
    mainOpen_projectAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "fileopen" ) ) );
    mainCreate_projectAction = new QAction( this, "mainCreate_projectAction" );
    projectExitAction = new QAction( this, "projectExitAction" );
    windowCloseAction = new QAction( this, "windowCloseAction" );
    windowClose_AllAction = new QAction( this, "windowClose_AllAction" );
    window_Action = new QAction( this, "window_Action" );
    windowTitleAction = new QAction( this, "windowTitleAction" );
    windowCascadeAction = new QAction( this, "windowCascadeAction" );
    windowTitle_HorizontallyAction = new QAction( this, "windowTitle_HorizontallyAction" );
    RVGDeleteAction = new QAction( this, "RVGDeleteAction" );
    RVGTestAction = new QAction( this, "RVGTestAction" );


    // toolbars
    toolBar = new QToolBar( QString(""), this, DockTop ); 

    RVGNewAction->addTo( toolBar );
    SaveAction->addTo( toolBar );
    helpAboutAction->addTo( toolBar );


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );

    MenuBar->setGeometry( QRect( 0, 0, 600, 24 ) );

    Project = new QPopupMenu( this );
    mainOpen_projectAction->addTo( Project );
    mainCreate_projectAction->addTo( Project );
    Project->insertSeparator();
    projectExitAction->addTo( Project );
    MenuBar->insertItem( QString(""), Project, 2 );

    RVG = new QPopupMenu( this );
    RVGNewAction->addTo( RVG );
    SaveAction->addTo( RVG );
    RVGDeleteAction->addTo( RVG );
    RVG->insertSeparator();
    RVGTestAction->addTo( RVG );
    MenuBar->insertItem( QString(""), RVG, 3 );

    Table = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), Table, 4 );

    Command = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), Command, 5 );

    Module = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), Module, 6 );

    Torturer = new QPopupMenu( this );
    MenuBar->insertItem( QString(""), Torturer, 7 );

    Window = new QPopupMenu( this );
    windowCloseAction->addTo( Window );
    windowClose_AllAction->addTo( Window );
    Window->insertSeparator();
    windowCascadeAction->addTo( Window );
    windowTitleAction->addTo( Window );
    windowTitle_HorizontallyAction->addTo( Window );
    Window->insertSeparator();
    MenuBar->insertItem( QString(""), Window, 8 );

    helpMenu = new QPopupMenu( this );
    helpContentsAction->addTo( helpMenu );
    helpIndexAction->addTo( helpMenu );
    helpMenu->insertSeparator();
    helpAboutAction->addTo( helpMenu );
    MenuBar->insertItem( QString(""), helpMenu, 9 );

    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( RVGNewAction, SIGNAL( activated() ), this, SLOT( newRVGAction() ) );
    connect( SaveAction, SIGNAL( activated() ), this, SLOT( SaveObjectAction() ) );
    connect( helpIndexAction, SIGNAL( activated() ), this, SLOT( helpIndex() ) );
    connect( helpContentsAction, SIGNAL( activated() ), this, SLOT( helpContents() ) );
    connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );
    connect( windowCloseAction, SIGNAL( activated() ), this, SLOT( windowCloseAction_activated() ) );
    connect( windowClose_AllAction, SIGNAL( activated() ), this, SLOT( windowClose_AllAction_activated() ) );
    connect( windowCascadeAction, SIGNAL( activated() ), this, SLOT( windowCascadeAction_activated() ) );
    connect( windowTitleAction, SIGNAL( activated() ), this, SLOT( windowTitleAction_activated() ) );
    connect( windowTitle_HorizontallyAction, SIGNAL( activated() ), this, SLOT( windowTitle_HorizontallyAction_activated() ) );
    connect( RVGDeleteAction, SIGNAL( activated() ), this, SLOT( RVGDeleteAction_activated() ) );
    connect( RVGTestAction, SIGNAL( activated() ), this, SLOT( RVGTestAction_activated() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MainWindow::~MainWindow()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MainWindow::languageChange()
{
    setCaption( tr( "Torturer Platform" ) );
    RVGNewAction->setText( tr( "New" ) );
    RVGNewAction->setMenuText( tr( "&New" ) );
    RVGNewAction->setAccel( tr( "Ctrl+N" ) );
    SaveAction->setText( tr( "Save" ) );
    SaveAction->setMenuText( tr( "&Save" ) );
    SaveAction->setAccel( tr( "Ctrl+S" ) );
    helpContentsAction->setText( tr( "Contents" ) );
    helpContentsAction->setMenuText( tr( "&Contents..." ) );
    helpContentsAction->setAccel( QString::null );
    helpIndexAction->setText( tr( "Index" ) );
    helpIndexAction->setMenuText( tr( "&Index..." ) );
    helpIndexAction->setAccel( QString::null );
    helpAboutAction->setText( tr( "About" ) );
    helpAboutAction->setMenuText( tr( "&About" ) );
    helpAboutAction->setAccel( QString::null );
    mainOpen_projectAction->setText( tr( "Open" ) );
    mainOpen_projectAction->setMenuText( tr( "Open" ) );
    mainCreate_projectAction->setText( tr( "Create" ) );
    mainCreate_projectAction->setMenuText( tr( "Create" ) );
    projectExitAction->setText( tr( "E&xit" ) );
    projectExitAction->setMenuText( tr( "E&xit" ) );
    windowCloseAction->setText( tr( "Cl&ose" ) );
    windowCloseAction->setMenuText( tr( "Cl&ose" ) );
    windowClose_AllAction->setText( tr( "Close Al&l" ) );
    windowClose_AllAction->setMenuText( tr( "Close Al&l" ) );
    window_Action->setText( tr( "-" ) );
    window_Action->setMenuText( tr( "-" ) );
    windowTitleAction->setText( tr( "&Title" ) );
    windowTitleAction->setMenuText( tr( "&Title" ) );
    windowCascadeAction->setText( tr( "Cascade" ) );
    windowCascadeAction->setMenuText( tr( "Cascade" ) );
    windowTitle_HorizontallyAction->setText( tr( "Title &Horizontally" ) );
    windowTitle_HorizontallyAction->setMenuText( tr( "Title &Horizontally" ) );
    RVGDeleteAction->setText( tr( "&Delete" ) );
    RVGDeleteAction->setMenuText( tr( "&Delete" ) );
    RVGTestAction->setText( tr( "&Test" ) );
    RVGTestAction->setMenuText( tr( "&Test" ) );
    toolBar->setLabel( tr( "Tools" ) );
    if (MenuBar->findItem(2))
        MenuBar->findItem(2)->setText( tr( "&Project" ) );
    if (MenuBar->findItem(3))
        MenuBar->findItem(3)->setText( tr( "&RVG" ) );
    if (MenuBar->findItem(4))
        MenuBar->findItem(4)->setText( tr( "&Table" ) );
    if (MenuBar->findItem(5))
        MenuBar->findItem(5)->setText( tr( "&Command" ) );
    if (MenuBar->findItem(6))
        MenuBar->findItem(6)->setText( tr( "&Module" ) );
    if (MenuBar->findItem(7))
        MenuBar->findItem(7)->setText( tr( "T&orturer" ) );
    if (MenuBar->findItem(8))
        MenuBar->findItem(8)->setText( tr( "&Window" ) );
    if (MenuBar->findItem(9))
        MenuBar->findItem(9)->setText( tr( "&Help" ) );
}

