/****************************************************************************
** Form implementation generated from reading ui file 'UI/rvgeditorui.ui'
**
** Created: Wed Feb 20 23:57:20 2008
**      by: The User Interface Compiler ($Id: rvgeditorui.cpp,v 1.2 2015/01/06 08:57:51 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "rvgeditorui.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtable.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "../UI/rvgeditorui.ui.h"
/*
 *  Constructs a RVGEditorUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
RVGEditorUI::RVGEditorUI( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "RVGEditorUI" );
    setMinimumSize( QSize( 600, 600 ) );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );

    mCommonGroupBox = new QGroupBox( centralWidget(), "mCommonGroupBox" );
    mCommonGroupBox->setGeometry( QRect( 20, 10, 500, 110 ) );

    mNameLabel = new QLabel( mCommonGroupBox, "mNameLabel" );
    mNameLabel->setGeometry( QRect( 20, 30, 66, 20 ) );

    mWeightLabel = new QLabel( mCommonGroupBox, "mWeightLabel" );
    mWeightLabel->setGeometry( QRect( 20, 70, 50, 20 ) );

    mValueTypeLabel = new QLabel( mCommonGroupBox, "mValueTypeLabel" );
    mValueTypeLabel->setGeometry( QRect( 280, 30, 70, 20 ) );

    mRVGTypeLabel = new QLabel( mCommonGroupBox, "mRVGTypeLabel" );
    mRVGTypeLabel->setGeometry( QRect( 280, 70, 60, 20 ) );

    mNamelineEdit = new QLineEdit( mCommonGroupBox, "mNamelineEdit" );
    mNamelineEdit->setGeometry( QRect( 90, 30, 123, 22 ) );

    mWeightlineEdit = new QLineEdit( mCommonGroupBox, "mWeightlineEdit" );
    mWeightlineEdit->setGeometry( QRect( 90, 70, 123, 22 ) );
    mWeightlineEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mWeightlineEdit->sizePolicy().hasHeightForWidth() ) );

    mTypeLineEdit = new QLineEdit( mCommonGroupBox, "mTypeLineEdit" );
    mTypeLineEdit->setEnabled( TRUE );
    mTypeLineEdit->setGeometry( QRect( 350, 30, 123, 22 ) );
    mTypeLineEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mTypeLineEdit->sizePolicy().hasHeightForWidth() ) );
    mTypeLineEdit->setReadOnly( TRUE );

    mRVGTypeLineEdit = new QLineEdit( mCommonGroupBox, "mRVGTypeLineEdit" );
    mRVGTypeLineEdit->setEnabled( TRUE );
    mRVGTypeLineEdit->setGeometry( QRect( 350, 70, 123, 22 ) );
    mRVGTypeLineEdit->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, mRVGTypeLineEdit->sizePolicy().hasHeightForWidth() ) );
    mRVGTypeLineEdit->setReadOnly( TRUE );

    mValuesGroupBox = new QGroupBox( centralWidget(), "mValuesGroupBox" );
    mValuesGroupBox->setGeometry( QRect( 20, 130, 500, 210 ) );
    mValuesGroupBox->setMinimumSize( QSize( 0, 0 ) );

    mValuesTable = new QTable( mValuesGroupBox, "mValuesTable" );
    mValuesTable->setGeometry( QRect( 10, 20, 335, 170 ) );
    mValuesTable->setNumRows( 0 );
    mValuesTable->setNumCols( 3 );

    mAddValueBtn = new QPushButton( mValuesGroupBox, "mAddValueBtn" );
    mAddValueBtn->setGeometry( QRect( 360, 30, 110, 24 ) );

    mDelValueBtn = new QPushButton( mValuesGroupBox, "mDelValueBtn" );
    mDelValueBtn->setGeometry( QRect( 360, 70, 110, 24 ) );

    mValueEditBtn = new QPushButton( mValuesGroupBox, "mValueEditBtn" );
    mValueEditBtn->setGeometry( QRect( 360, 110, 110, 24 ) );

    mOtherGroupBox = new QGroupBox( centralWidget(), "mOtherGroupBox" );
    mOtherGroupBox->setGeometry( QRect( 20, 350, 500, 110 ) );

    mMinLabel = new QLabel( mOtherGroupBox, "mMinLabel" );
    mMinLabel->setGeometry( QRect( 20, 30, 66, 20 ) );

    mMaxLabel = new QLabel( mOtherGroupBox, "mMaxLabel" );
    mMaxLabel->setGeometry( QRect( 20, 70, 66, 20 ) );

    mUniqueCheckBox = new QCheckBox( mOtherGroupBox, "mUniqueCheckBox" );
    mUniqueCheckBox->setGeometry( QRect( 250, 30, 230, 20 ) );

    mOrderedCheckBox = new QCheckBox( mOtherGroupBox, "mOrderedCheckBox" );
    mOrderedCheckBox->setGeometry( QRect( 250, 70, 220, 20 ) );

    mMinLineEdit = new QLineEdit( mOtherGroupBox, "mMinLineEdit" );
    mMinLineEdit->setGeometry( QRect( 90, 30, 123, 22 ) );

    mMaxLineEdit = new QLineEdit( mOtherGroupBox, "mMaxLineEdit" );
    mMaxLineEdit->setGeometry( QRect( 90, 70, 123, 22 ) );

    mSpecialGroupBox = new QGroupBox( centralWidget(), "mSpecialGroupBox" );
    mSpecialGroupBox->setGeometry( QRect( 20, 470, 500, 70 ) );

    mSeperatorLabel = new QLabel( mSpecialGroupBox, "mSeperatorLabel" );
    mSeperatorLabel->setGeometry( QRect( 20, 30, 66, 20 ) );

    mSeperatorComboBox = new QComboBox( FALSE, mSpecialGroupBox, "mSeperatorComboBox" );
    mSeperatorComboBox->setGeometry( QRect( 90, 30, 120, 22 ) );

    // toolbars

    languageChange();
    resize( QSize(600, 600).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( this, SIGNAL( usesTextLabelChanged(bool) ), this, SLOT( RVGEditorUI_usesTextLabelChanged(bool) ) );
    connect( mAddValueBtn, SIGNAL( clicked() ), this, SLOT( AddValueBtn_clicked() ) );
    connect( mDelValueBtn, SIGNAL( clicked() ), this, SLOT( DelValueBtn_clicked() ) );
    connect( mValueEditBtn, SIGNAL( clicked() ), this, SLOT( ValueEditBtn_clicked() ) );
    connect( mNamelineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( NamelineEdit_textChanged(const QString&) ) );
    connect( mWeightlineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( WeightlineEdit_textChanged(const QString&) ) );
    connect( mTypeLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( TypeLineEdit_textChanged(const QString&) ) );
    connect( mRVGTypeLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( RVGTypeLineEdit_textChanged(const QString&) ) );
    connect( mMinLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( MinLineEdit_textChanged(const QString&) ) );
    connect( mSeperatorComboBox, SIGNAL( textChanged(const QString&) ), this, SLOT( SeperatorComboBox_textChanged(const QString&) ) );
    connect( mMaxLineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( MaxLineEdit_textChanged(const QString&) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
RVGEditorUI::~RVGEditorUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RVGEditorUI::languageChange()
{
    setCaption( tr( "RVG Editor" ) );
    mCommonGroupBox->setTitle( tr( "Common" ) );
    mNameLabel->setText( tr( "Name" ) );
    mWeightLabel->setText( tr( "Weight" ) );
    mValueTypeLabel->setText( tr( "ValueType" ) );
    mRVGTypeLabel->setText( tr( "RVGType" ) );
    mValuesGroupBox->setTitle( tr( "ValueSelector" ) );
    mAddValueBtn->setText( tr( "Add" ) );
    mDelValueBtn->setText( tr( "Delete" ) );
    mValueEditBtn->setText( tr( "Edit" ) );
    mOtherGroupBox->setTitle( tr( "Additional" ) );
    mMinLabel->setText( tr( "MinNum" ) );
    mMaxLabel->setText( tr( "MaxNum" ) );
    mUniqueCheckBox->setText( tr( "Uniquely select from ValueSelecor" ) );
    mOrderedCheckBox->setText( tr( "Keep in order as ValueSelector" ) );
    mSpecialGroupBox->setTitle( tr( "Special" ) );
    mSeperatorLabel->setText( tr( "Seperator" ) );
}

