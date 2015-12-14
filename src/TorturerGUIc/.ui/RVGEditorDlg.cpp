/****************************************************************************
** Form implementation generated from reading ui file 'UI/RVGEditorDlg.ui'
**
** Created: Sun Mar 16 15:36:23 2008
**      by: The User Interface Compiler ($Id: RVGEditorDlg.cpp,v 1.2 2015/01/06 08:57:52 andy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "RVGEditorDlg.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qtable.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a RVGEditorDlg as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
RVGEditorDlg::RVGEditorDlg( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "RVGEditorDlg" );
    setSizeGripEnabled( TRUE );

    mValuesGroupBox = new QGroupBox( this, "mValuesGroupBox" );
    mValuesGroupBox->setGeometry( QRect( 20, 130, 500, 210 ) );
    mValuesGroupBox->setMinimumSize( QSize( 0, 0 ) );

    mValueEditBtn = new QPushButton( mValuesGroupBox, "mValueEditBtn" );
    mValueEditBtn->setGeometry( QRect( 360, 110, 110, 24 ) );

    mAddValueBtn = new QPushButton( mValuesGroupBox, "mAddValueBtn" );
    mAddValueBtn->setGeometry( QRect( 360, 30, 110, 24 ) );

    mDelValueBtn = new QPushButton( mValuesGroupBox, "mDelValueBtn" );
    mDelValueBtn->setGeometry( QRect( 360, 70, 110, 24 ) );

    mValuesTable = new QTable( mValuesGroupBox, "mValuesTable" );
    mValuesTable->setGeometry( QRect( 10, 20, 335, 170 ) );
    mValuesTable->setNumRows( 0 );
    mValuesTable->setNumCols( 3 );

    mOtherGroupBox = new QGroupBox( this, "mOtherGroupBox" );
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

    mSpecialGroupBox = new QGroupBox( this, "mSpecialGroupBox" );
    mSpecialGroupBox->setGeometry( QRect( 20, 470, 500, 70 ) );

    mSeperatorLabel = new QLabel( mSpecialGroupBox, "mSeperatorLabel" );
    mSeperatorLabel->setGeometry( QRect( 20, 30, 66, 20 ) );

    mSeperatorLineEdit = new QLineEdit( mSpecialGroupBox, "mSeperatorLineEdit" );
    mSeperatorLineEdit->setGeometry( QRect( 90, 30, 123, 22 ) );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout1" );
    privateLayoutWidget->setGeometry( QRect( 40, 560, 476, 33 ) );
    Layout1 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout1"); 
    Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( Horizontal_Spacing2 );

    buttonOk = new QPushButton( privateLayoutWidget, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( privateLayoutWidget, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );

    mCommonGroupBox = new QGroupBox( this, "mCommonGroupBox" );
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

    mTypeComboBox = new QComboBox( FALSE, mCommonGroupBox, "mTypeComboBox" );
    mTypeComboBox->setGeometry( QRect( 350, 30, 120, 22 ) );

    mRVGTypeComboBox = new QComboBox( FALSE, mCommonGroupBox, "mRVGTypeComboBox" );
    mRVGTypeComboBox->setGeometry( QRect( 350, 70, 120, 22 ) );
    languageChange();
    resize( QSize(536, 609).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
RVGEditorDlg::~RVGEditorDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RVGEditorDlg::languageChange()
{
    setCaption( tr( "RVG Editor Dialog" ) );
    mValuesGroupBox->setTitle( tr( "ValueSelector" ) );
    mValueEditBtn->setText( tr( "Edit" ) );
    mAddValueBtn->setText( tr( "Add" ) );
    mDelValueBtn->setText( tr( "Delete" ) );
    mOtherGroupBox->setTitle( tr( "Additional" ) );
    mMinLabel->setText( tr( "MinNum" ) );
    mMaxLabel->setText( tr( "MaxNum" ) );
    mUniqueCheckBox->setText( tr( "Uniquely select from ValueSelecor" ) );
    mOrderedCheckBox->setText( tr( "Keep in order as ValueSelector" ) );
    mSpecialGroupBox->setTitle( tr( "Special" ) );
    mSeperatorLabel->setText( tr( "Seperator" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    mCommonGroupBox->setTitle( tr( "Common" ) );
    mNameLabel->setText( tr( "Name" ) );
    mWeightLabel->setText( tr( "Weight" ) );
    mValueTypeLabel->setText( tr( "ValueType" ) );
    mRVGTypeLabel->setText( tr( "RVGType" ) );
}

