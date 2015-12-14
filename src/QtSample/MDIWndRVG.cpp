////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// Wednesday, January 30, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#include "MDIWndRVG.h"
#include "RVG/CharsetRSG.h"
#include "RVG/ComposeRSG.h"
#include "RVG/EnumSeqRSG.h"

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
#include <qscrollview.h>


MDIWndRVG::MDIWndRVG(AosRVGPtr& rvg,  QWidget* parent, const char* name, WFlags fl)
:MDIWindow(parent, name, fl),
mRVG(rvg)
{
	mTreeItem = NULL;
    (void)statusBar();
    if ( !name )
	setName( "MDIWndRVG" );
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

    mTypeLineEdit = new QComboBox( FALSE, mCommonGroupBox, "mTypeLineEdit" );
    mTypeLineEdit->setEnabled( FALSE );
    mTypeLineEdit->setGeometry( QRect( 350, 30, 123, 22 ) );

    mRVGTypeLineEdit = new QComboBox( FALSE, mCommonGroupBox, "mRVGTypeLineEdit" );
    mRVGTypeLineEdit->setEnabled( FALSE );
    mRVGTypeLineEdit->setGeometry( QRect( 350, 70, 123, 22 ) );

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

    mSeperatorComboBox = new QLineEdit( FALSE, mSpecialGroupBox, "mSeperatorComboBox" );
    mSeperatorComboBox->setGeometry( QRect( 90, 30, 120, 22 ) );

    // toolbars

    languageChange();
    resize( QSize(600, 600).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( this, SIGNAL( usesTextLabelChanged(bool) ), this, SLOT( ValuesTable_TextLabelChanged(bool) ) );
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

	if (mRVG)
	{
		init();
	}
	mIsModified = false;
}


MDIWndRVG::~MDIWndRVG()
{
}


void 
MDIWndRVG::languageChange()
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


void 
MDIWndRVG::init()
{
	mNamelineEdit->setText(mRVG->getName());

	QString str;
	str.sprintf("%u", mRVG->getWeight());
	mWeightlineEdit->setText(str);

	int i;
	for (i=1; i<AosValueType::eLastEntry; i++)
	{
		mTypeLineEdit->insertItem(AosValueType::enum2Str((AosValueType::E)i));
	}
	str = AosValueType::enum2Str(mRVG->getValueType());
	mTypeLineEdit->setCurrentText(str);

	for (i=1; i<AosRVGType::eLastEntry; i++)
	{
		mRVGTypeLineEdit->insertItem(AosRVGType::enum2Str((AosRVGType::E)i));
	}
	str = AosRVGType::enum2Str(mRVG->getRVGType());
	mRVGTypeLineEdit->setCurrentText(str);

	switch (mRVG->getRVGType())
	{
		case AosRVGType::eCharsetRSG:
			{
				mValueEditBtn->hide();
				mSpecialGroupBox->hide();
				mValuesTable->setNumCols(2);
				mValuesTable->setColumnStretchable(0, TRUE);
				mValuesTable->setColumnStretchable(1, TRUE);
				QStringList strList;
				strList.append("Char");
				strList.append("Weight");
				mValuesTable->setColumnLabels(strList);
				AosCharsetRSGPtr ptr = dynamic_cast<AosCharsetRSG*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosCharsetRSG::AosCharsetWeightList charset;
				ptr->getCharset(charset);
				mValuesTable->setNumRows(charset.size());
				int i=0;
				QString str;
				AosCharsetRSG::AosCharsetWeightList::iterator iter;
				for (iter=charset.begin(); iter!=charset.end(); iter++, i++)
				{
					str.sprintf("%c", iter->mChar);
					mValuesTable->setText(i, 0, str);	
					str.sprintf("%u", iter->mWeight);
					mValuesTable->setText(i, 1, str);	
				}
				str.sprintf("%u", ptr->getMinLen());
				mMinLineEdit->setText(str);
				str.sprintf("%u", ptr->getMaxLen());
				mMaxLineEdit->setText(str);
				mUniqueCheckBox->setChecked(ptr->getUniqueFlag());
				mOrderedCheckBox->setChecked(ptr->getOrderedFlag());
			}
			break;
		case AosRVGType::eEnumRSG:
			{
				mValueEditBtn->hide();
				mOtherGroupBox->hide();
				mSpecialGroupBox->hide();
				mValuesTable->setNumCols(2);
				mValuesTable->setColumnStretchable(0, TRUE);
				mValuesTable->setColumnStretchable(1, TRUE);
				QStringList strList;
				strList.append("String");
				strList.append("Weight");
				mValuesTable->setColumnLabels(strList);
				AosEnumRSGPtr ptr = dynamic_cast<AosEnumRSG*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosEnumRSG::AosEnumStrWeightList enumList;
				ptr->getEnumString(enumList);
				mValuesTable->setNumRows(enumList.size());
				int i=0;
				QString str;
				AosEnumRSG::AosEnumStrWeightList::iterator iter;
				for (iter=enumList.begin(); iter!=enumList.end(); iter++, i++)
				{
					mValuesTable->setText(i, 0, iter->enumStr);	
					str.sprintf("%u", iter->weight);
					mValuesTable->setText(i, 1, str);	
				}
			}
			break;
		case AosRVGType::eEnumSeqRSG:
			{
				mValueEditBtn->hide();
				mValuesTable->setNumCols(2);
				mValuesTable->setColumnStretchable(0, TRUE);
				mValuesTable->setColumnStretchable(1, TRUE);
				QStringList strList;
				strList.append("String");
				strList.append("Weight");
				mValuesTable->setColumnLabels(strList);
				AosEnumSeqRSGPtr ptr = dynamic_cast<AosEnumSeqRSG*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosEnumRSG::AosEnumStrWeightList enumList;
				ptr->getEnumString(enumList);
				mValuesTable->setNumRows(enumList.size());
				int i=0;
				QString str;
				AosEnumRSG::AosEnumStrWeightList::iterator iter;
				for (iter=enumList.begin(); iter!=enumList.end(); iter++, i++)
				{
					mValuesTable->setText(i, 0, iter->enumStr);	
					str.sprintf("%u", iter->weight);
					mValuesTable->setText(i, 1, str);	
				}
				str.sprintf("%u", ptr->getMinNum());
				mMinLineEdit->setText(str);
				str.sprintf("%u", ptr->getMaxNum());
				mMaxLineEdit->setText(str);
				mUniqueCheckBox->setChecked(ptr->getUniqueFlag());
				mOrderedCheckBox->setChecked(ptr->getOrderedFlag());
				str.sprintf("%c", ptr->getSeparator());
				mSeperatorComboBox->setText(str);
			}
			break;
		case AosRVGType::eAndRSG:
		case AosRVGType::eOrRSG:
			{
				mOtherGroupBox->hide();
				mSpecialGroupBox->hide();
				mValuesTable->setNumCols(2);
				mValuesTable->setColumnStretchable(0, TRUE);
				mValuesTable->setColumnStretchable(1, TRUE);
				QStringList strList;
				strList.append("RVG");
				strList.append("Weight");
				mValuesTable->setColumnLabels(strList);
				AosComposeRSGPtr ptr = dynamic_cast<AosComposeRSG*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosRVGPtrList rvgList;
				ptr->getBRSGList(rvgList);
				mValuesTable->setNumRows(rvgList.size());
				int i=0;
				AosRVGPtrList::iterator iter;
				for (iter=rvgList.begin(); iter!=rvgList.end(); iter++, i++)
				{
					mValuesTable->setText(i, 0, (*iter)->getName());	
					QString str;
					str.sprintf("%u", (*iter)->getWeight());
					mValuesTable->setText(i, 1, str);	
				}
			}
			break;
		case AosRVGType::eU8RIG:
			{
				initRIGValueSelectorTableHeader();
				AosU8RIGPtr ptr = dynamic_cast<AosRIG<u8>*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosRIG<u8>::AosIntPairList pairList;
				ptr->getIntegerPair(pairList);
				mValuesTable->setNumRows(pairList.size());
				int i=0;
				AosRIG<u8>::AosIntPairList::iterator iter;
				for (iter=pairList.begin(); iter!=pairList.end(); iter++, i++)
				{
					QString str;
					str.sprintf("%u", iter->mLow);
					mValuesTable->setText(i, 0, str);	
					str.sprintf("%u", iter->mHigh);
					mValuesTable->setText(i, 1, str);	
					str.sprintf("%u", iter->mWeight);
					mValuesTable->setText(i, 2, str);	
				}
			}
			break;
		case AosRVGType::eU16RIG:
			{
				initRIGValueSelectorTableHeader();
				AosU16RIGPtr ptr = dynamic_cast<AosRIG<u16>*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosRIG<u16>::AosIntPairList pairList;
				ptr->getIntegerPair(pairList);
				mValuesTable->setNumRows(pairList.size());
				int i=0;
				AosRIG<u16>::AosIntPairList::iterator iter;
				for (iter=pairList.begin(); iter!=pairList.end(); iter++, i++)
				{
					QString str;
					str.sprintf("%u", iter->mLow);
					mValuesTable->setText(i, 0, str);	
					str.sprintf("%u", iter->mHigh);
					mValuesTable->setText(i, 1, str);	
					str.sprintf("%u", iter->mWeight);
					mValuesTable->setText(i, 2, str);	
				}
			}
			break;
		case AosRVGType::eU32RIG:
			{
				initRIGValueSelectorTableHeader();
				AosU32RIGPtr ptr = dynamic_cast<AosRIG<u32>*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosRIG<u32>::AosIntPairList pairList;
				ptr->getIntegerPair(pairList);
				mValuesTable->setNumRows(pairList.size());
				int i=0;
				AosRIG<u32>::AosIntPairList::iterator iter;
				for (iter=pairList.begin(); iter!=pairList.end(); iter++, i++)
				{
					QString str;
					str.sprintf("%u", iter->mLow);
					mValuesTable->setText(i, 0, str);	
					str.sprintf("%u", iter->mHigh);
					mValuesTable->setText(i, 1, str);	
					str.sprintf("%u", iter->mWeight);
					mValuesTable->setText(i, 2, str);	
				}
			}
			break;
		case AosRVGType::eU64RIG:
			{
				initRIGValueSelectorTableHeader();
				AosU64RIGPtr ptr = dynamic_cast<AosRIG<u64>*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosRIG<u64>::AosIntPairList pairList;
				ptr->getIntegerPair(pairList);
				mValuesTable->setNumRows(pairList.size());
				int i=0;
				AosRIG<u64>::AosIntPairList::iterator iter;
				for (iter=pairList.begin(); iter!=pairList.end(); iter++, i++)
				{
					QString str;
					str.sprintf("%llu", iter->mLow);
					mValuesTable->setText(i, 0, str);	
					str.sprintf("%llu", iter->mHigh);
					mValuesTable->setText(i, 1, str);	
					str.sprintf("%u", iter->mWeight);
					mValuesTable->setText(i, 2, str);	
				}
			}
			break;
		case AosRVGType::eInt8RIG:
			{
				initRIGValueSelectorTableHeader();
				AosInt8RIGPtr ptr = dynamic_cast<AosRIG<int8_t>*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosRIG<int8_t>::AosIntPairList pairList;
				ptr->getIntegerPair(pairList);
				mValuesTable->setNumRows(pairList.size());
				int i=0;
				AosRIG<int8_t>::AosIntPairList::iterator iter;
				for (iter=pairList.begin(); iter!=pairList.end(); iter++, i++)
				{
					QString str;
					str.sprintf("%d", iter->mLow);
					mValuesTable->setText(i, 0, str);	
					str.sprintf("%d", iter->mHigh);
					mValuesTable->setText(i, 1, str);	
					str.sprintf("%u", iter->mWeight);
					mValuesTable->setText(i, 2, str);	
				}
			}
			break;
		case AosRVGType::eInt16RIG:
			{
				initRIGValueSelectorTableHeader();
				AosInt16RIGPtr ptr = dynamic_cast<AosRIG<int16_t>*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosRIG<int16_t>::AosIntPairList pairList;
				ptr->getIntegerPair(pairList);
				mValuesTable->setNumRows(pairList.size());
				int i=0;
				AosRIG<int16_t>::AosIntPairList::iterator iter;
				for (iter=pairList.begin(); iter!=pairList.end(); iter++, i++)
				{
					QString str;
					str.sprintf("%d", iter->mLow);
					mValuesTable->setText(i, 0, str);	
					str.sprintf("%d", iter->mHigh);
					mValuesTable->setText(i, 1, str);	
					str.sprintf("%u", iter->mWeight);
					mValuesTable->setText(i, 2, str);	
				}
			}
			break;
		case AosRVGType::eInt32RIG:
			{
				initRIGValueSelectorTableHeader();
				AosInt32RIGPtr ptr = dynamic_cast<AosRIG<int32_t>*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosRIG<int32_t>::AosIntPairList pairList;
				ptr->getIntegerPair(pairList);
				mValuesTable->setNumRows(pairList.size());
				int i=0;
				AosRIG<int32_t>::AosIntPairList::iterator iter;
				for (iter=pairList.begin(); iter!=pairList.end(); iter++, i++)
				{
					QString str;
					str.sprintf("%d", iter->mLow);
					mValuesTable->setText(i, 0, str);	
					str.sprintf("%d", iter->mHigh);
					mValuesTable->setText(i, 1, str);	
					str.sprintf("%u", iter->mWeight);
					mValuesTable->setText(i, 2, str);	
				}
			}
			break;
		case AosRVGType::eInt64RIG:
			{
				initRIGValueSelectorTableHeader();
				AosInt64RIGPtr ptr = dynamic_cast<AosRIG<int64_t>*>(mRVG.getPtr());
				if (!ptr)
				{
					break;
				}
				AosRIG<int64_t>::AosIntPairList pairList;
				ptr->getIntegerPair(pairList);
				mValuesTable->setNumRows(pairList.size());
				int i=0;
				AosRIG<int64_t>::AosIntPairList::iterator iter;
				for (iter=pairList.begin(); iter!=pairList.end(); iter++, i++)
				{
					QString str;
					str.sprintf("%lld", iter->mLow);
					mValuesTable->setText(i, 0, str);	
					str.sprintf("%lld", iter->mHigh);
					mValuesTable->setText(i, 1, str);	
					str.sprintf("%u", iter->mWeight);
					mValuesTable->setText(i, 2, str);	
				}
			}
			break;
		case AosRVGType::eFloat:
			mValueEditBtn->hide();
			mOtherGroupBox->hide();
			mSpecialGroupBox->hide();
			break;
		case AosRVGType::eDouble:
			mValueEditBtn->hide();
			mOtherGroupBox->hide();
			mSpecialGroupBox->hide();
			break;
		default:
			mValueEditBtn->hide();
			mCommonGroupBox->hide();
			mOtherGroupBox->hide();
			mSpecialGroupBox->hide();
			break;
	}
}


void 
MDIWndRVG::initRIGValueSelectorTableHeader()
{
	mValueEditBtn->hide();
	mOtherGroupBox->hide();
	mSpecialGroupBox->hide();
	mValuesTable->setNumCols(3);
	mValuesTable->setColumnStretchable(2, TRUE);
	QStringList strList;
	strList.append("From");
	strList.append("To");
	strList.append("Weight");
	mValuesTable->setColumnLabels(strList);
}


void 
MDIWndRVG::save()
{
	mIsModified = false;
	if (!mRVG)
	{
		return;
	}
	//
	// check all the data in the widgets whether the format is correct or incorrect
	//
//	if (!checkWidgetDataFormat())
//	{
//		return;
//	}

	mRVG->setName(mNamelineEdit->text().ascii());
	mRVG->setWeight(mWeightlineEdit->text().toUInt());
	switch (mRVG->getRVGType())
	{
		case AosRVGType::eCharsetRSG:
			{
				AosCharsetRSGPtr rvg = dynamic_cast<AosCharsetRSG*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearCharset();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					if (!mValuesTable->text(row, 0).isEmpty())
					{
						rvg->addChar(mValuesTable->text(row, 0).at(0).latin1(), mValuesTable->text(row, 1).toUInt());
					}
				}

				//
				// save other features 
				//
				rvg->setMinLen(mMinLineEdit->text().toUInt());
				rvg->setMaxLen(mMaxLineEdit->text().toUInt());
				rvg->setUniqueFlag(mUniqueCheckBox->isChecked());
				rvg->setOrderedFlag(mOrderedCheckBox->isChecked());
			}
			break;
		case AosRVGType::eEnumRSG:
			{
				AosEnumRSGPtr rvg = dynamic_cast<AosEnumRSG*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearEnumString();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setEnumString(mValuesTable->text(row, 0).ascii(), mValuesTable->text(row, 1).toUInt());
				}
			}
			break;
		case AosRVGType::eEnumSeqRSG:
			{
				AosEnumSeqRSGPtr rvg = dynamic_cast<AosEnumSeqRSG*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearEnumString();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setEnumString(mValuesTable->text(row, 0).ascii(), mValuesTable->text(row, 1).toUInt());
				}
				//
				// save other features 
				//
				rvg->setMinNum(mMinLineEdit->text().toUInt());
				rvg->setMaxNum(mMaxLineEdit->text().toUInt());
				rvg->setUniqueFlag(mUniqueCheckBox->isChecked());
				rvg->setOrderedFlag(mOrderedCheckBox->isChecked());
				if (!mSeperatorComboBox->text().isEmpty())
				{
					rvg->setSeperator(mSeperatorComboBox->text().at(0).latin1());
				}
				else
				{
					rvg->setSeperator(' ');
				}
			}
			break;
		case AosRVGType::eAndRSG:
		case AosRVGType::eOrRSG:
			break;
		case AosRVGType::eU8RIG:
			{
				AosU8RIGPtr rvg = dynamic_cast<AosRIG<u8>*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearIntegerPair();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setIntegerPair(mValuesTable->text(row, 0).toUInt(), mValuesTable->text(row, 1).toUInt(), mValuesTable->text(row, 2).toUInt());
				}
			}
			break;
		case AosRVGType::eU16RIG:
			{
				AosU16RIGPtr rvg = dynamic_cast<AosRIG<u16>*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearIntegerPair();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setIntegerPair(mValuesTable->text(row, 0).toUShort(), mValuesTable->text(row, 1).toUShort(), mValuesTable->text(row, 2).toUInt());
				}
			}
			break;
		case AosRVGType::eU32RIG:
			{
				AosU16RIGPtr rvg = dynamic_cast<AosRIG<u16>*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearIntegerPair();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setIntegerPair(mValuesTable->text(row, 0).toUInt(), mValuesTable->text(row, 1).toUInt(), mValuesTable->text(row, 2).toUInt());
				}
			}
			break;
		case AosRVGType::eU64RIG:
			{
				AosU16RIGPtr rvg = dynamic_cast<AosRIG<u16>*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearIntegerPair();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setIntegerPair(mValuesTable->text(row, 0).toULongLong(), mValuesTable->text(row, 1).toULongLong(), mValuesTable->text(row, 2).toUInt());
				}
			}
			break;
		case AosRVGType::eInt8RIG:
			{
				AosU16RIGPtr rvg = dynamic_cast<AosRIG<u16>*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearIntegerPair();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setIntegerPair(mValuesTable->text(row, 0).toInt(), mValuesTable->text(row, 1).toInt(), mValuesTable->text(row, 2).toUInt());
				}
			}
			break;
		case AosRVGType::eInt16RIG:
			{
				AosU16RIGPtr rvg = dynamic_cast<AosRIG<u16>*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearIntegerPair();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setIntegerPair(mValuesTable->text(row, 0).toShort(), mValuesTable->text(row, 1).toShort(), mValuesTable->text(row, 2).toUInt());
				}
			}
			break;
		case AosRVGType::eInt32RIG:
			{
				AosU16RIGPtr rvg = dynamic_cast<AosRIG<u16>*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearIntegerPair();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setIntegerPair(mValuesTable->text(row, 0).toInt(), mValuesTable->text(row, 1).toInt(), mValuesTable->text(row, 2).toUInt());
				}
			}
			break;
		case AosRVGType::eInt64RIG:
			{
				AosInt64RIGPtr rvg = dynamic_cast<AosRIG<int64_t>*>(mRVG.getPtr());
				if (!rvg)
				{
					break;
				}
				//
				// save ValueSelector
				//
				rvg->clearIntegerPair();
				for (int row=0; row<mValuesTable->numRows(); row++)
				{
					rvg->setIntegerPair(mValuesTable->text(row, 0).toLongLong(), mValuesTable->text(row, 1).toLongLong(), mValuesTable->text(row, 2).toUInt());
				}
			}
			break;
		case AosRVGType::eFloat:
			break;
		case AosRVGType::eDouble:
			break;
		default:
			return;
	}
	//
	// update the left object tree list
	//
	if (mTreeItem == NULL)
	{
		AosBaseTreeItem *item = AosBaseTreeItem::getTreeItem(mRVG->getProductName(), "", eTreeItem_RVGs);
		mTreeItem = new AosRVGTreeItem(item, mRVG);
	}
	else
	{
		if (mTreeItem->text(0) != mRVG->getName())
		{
			//
			// delete the original rvg xml config file
			//
			mRVG->setName(mTreeItem->text(0).ascii());
			mRVG->deleteConfigFile();
			mRVG->setName(mNamelineEdit->text().ascii());
			//
			// the rvg name has changed, need to update the tree item name
			//
			mTreeItem->setText(0, mRVG->getName());
		}
	}
	mRVG->writeToFile();
}


void 
MDIWndRVG::saveAs()
{
}


void 
MDIWndRVG::ValuesTable_TextLabelChanged( bool )
{
	mIsModified = true;

}


void 
MDIWndRVG::AddValueBtn_clicked()
{
	mValuesTable->insertRows(mValuesTable->numRows());
}


void 
MDIWndRVG::DelValueBtn_clicked()
{
	mValuesTable->removeRow(mValuesTable->currentRow());
}


void 
MDIWndRVG::ValueEditBtn_clicked()
{

}


void 
MDIWndRVG::NamelineEdit_textChanged( const QString & )
{
	mIsModified = true;

}


void 
MDIWndRVG::WeightlineEdit_textChanged( const QString & )
{
	mIsModified = true;

}


void 
MDIWndRVG::TypeLineEdit_textChanged( const QString & )
{
	mIsModified = true;

}


void 
MDIWndRVG::RVGTypeLineEdit_textChanged( const QString & )
{
}


void 
MDIWndRVG::MinLineEdit_textChanged( const QString & )
{
	mIsModified = true;

}


void 
MDIWndRVG::SeperatorComboBox_textChanged( const QString & )
{
	mIsModified = true;
}


void 
MDIWndRVG::MaxLineEdit_textChanged( const QString & )
{
	mIsModified = true;
}


AosRVGPtr 
MDIWndRVG::getRVG()
{
	return mRVG;
}


AosRVGTreeItem* 
MDIWndRVG::getTreeItem()
{
	return mTreeItem;
}


void 
MDIWndRVG::setTreeItem(AosRVGTreeItem* item)
{
	mTreeItem = item;
}
