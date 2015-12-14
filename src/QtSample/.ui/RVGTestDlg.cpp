/****************************************************************************
** Form implementation generated from reading ui file 'UI/RVGTestDlg.ui'
**
** Created: Wed Feb 20 23:57:27 2008
**      by: The User Interface Compiler ($Id: RVGTestDlg.cpp,v 1.2 2015/01/06 08:57:51 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "RVGTestDlg.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "../UI/RVGTestDlg.ui.h"
/*
 *  Constructs a RVGTestDlg as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
RVGTestDlg::RVGTestDlg( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "RVGTestDlg" );
    setSizeGripEnabled( TRUE );

    mRVGGroupBox = new QGroupBox( this, "mRVGGroupBox" );
    mRVGGroupBox->setGeometry( QRect( 20, 10, 480, 70 ) );

    mRVGNameLabel = new QLabel( mRVGGroupBox, "mRVGNameLabel" );
    mRVGNameLabel->setGeometry( QRect( 250, 30, 66, 20 ) );

    mProductNameLineEdit = new QLineEdit( mRVGGroupBox, "mProductNameLineEdit" );
    mProductNameLineEdit->setGeometry( QRect( 120, 30, 100, 22 ) );
    mProductNameLineEdit->setReadOnly( TRUE );

    mRVGNamelineEdit = new QLineEdit( mRVGGroupBox, "mRVGNamelineEdit" );
    mRVGNamelineEdit->setGeometry( QRect( 330, 30, 130, 21 ) );
    mRVGNamelineEdit->setReadOnly( TRUE );

    mProductNameLabel = new QLabel( mRVGGroupBox, "mProductNameLabel" );
    mProductNameLabel->setGeometry( QRect( 10, 30, 90, 20 ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout1" );
    privateLayoutWidget->setGeometry( QRect( 140, 480, 360, 33 ) );
    Layout1 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout1"); 
    Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( Horizontal_Spacing2 );

    buttonOk = new QPushButton( privateLayoutWidget, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    mPolicyGroupBox = new QGroupBox( this, "mPolicyGroupBox" );
    mPolicyGroupBox->setGeometry( QRect( 20, 90, 480, 380 ) );

    mTestTimes = new QLabel( mPolicyGroupBox, "mTestTimes" );
    mTestTimes->setGeometry( QRect( 10, 30, 66, 20 ) );

    mValuesLabel = new QLabel( mPolicyGroupBox, "mValuesLabel" );
    mValuesLabel->setGeometry( QRect( 10, 70, 120, 20 ) );

    mValuesListBox = new QListBox( mPolicyGroupBox, "mValuesListBox" );
    mValuesListBox->setGeometry( QRect( 10, 100, 450, 270 ) );
    mValuesListBox->setColumnMode( QListBox::FitToWidth );

    mTestTimesLineEdit = new QLineEdit( mPolicyGroupBox, "mTestTimesLineEdit" );
    mTestTimesLineEdit->setGeometry( QRect( 120, 30, 100, 22 ) );

    mStartTestBtn = new QPushButton( mPolicyGroupBox, "mStartTestBtn" );
    mStartTestBtn->setGeometry( QRect( 248, 30, 210, 24 ) );
    languageChange();
    resize( QSize(517, 525).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( mStartTestBtn, SIGNAL( clicked() ), this, SLOT( startTestBtn_clicked() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
RVGTestDlg::~RVGTestDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RVGTestDlg::languageChange()
{
    setCaption( tr( "Test RVG" ) );
    mRVGGroupBox->setTitle( tr( "Tested RVG" ) );
    mRVGNameLabel->setText( tr( "RVG Name" ) );
    mProductNameLineEdit->setText( QString::null );
    mProductNameLabel->setText( tr( "Product Name" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    mPolicyGroupBox->setTitle( tr( "Test Info" ) );
    mTestTimes->setText( tr( "Test Times" ) );
    mValuesLabel->setText( tr( "Generated Values" ) );
    mValuesListBox->clear();
    mValuesListBox->insertItem( tr( "New Item" ) );
    mTestTimesLineEdit->setText( tr( "100" ) );
    mStartTestBtn->setText( tr( "Start Testing" ) );
}

