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
// Modification History:
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Funcs/GenFunc.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Mutex.h"


AosGenFunc::AosGenFunc(
		const u32 type,
		const int version)
:
AosJimoGenFunc(AosJimoType::eFunction, version),
mFuncName("")
{
	mSize = 0;
	mConstValues = NULL;
	mExprValues = NULL;
}

AosGenFunc::AosGenFunc(
		const OmnString &fname,
		const int version)
:
AosJimoGenFunc(AosJimoType::eFunction, version),
mFuncName(fname)
{
	mSize = 0;
	mConstValues = NULL;
	mExprValues = NULL;
}


AosGenFunc::AosGenFunc(const AosGenFunc &rhs)
:
AosJimoGenFunc(rhs),
mSize(rhs.mSize),
mFuncName(rhs.mFuncName),
mConstValues(NULL),
mExprValues(NULL)
{
	if (mSize > 0)
	{
		mConstValues = OmnNew AosValueRslt[mSize];
		mExprValues = OmnNew AosExprObj*[mSize];
		memset(mExprValues, 0, sizeof(AosExprObj*) * mSize);

		for (u32 i = 0; i < mSize; i++)
		{
			mParms.push_back(rhs.mParms[i]->cloneExpr());
			if (mParms[i]->isConstant())
			{
				mConstValues[i] = rhs.mConstValues[i];
			}
			else
			{
				mExprValues[i] = mParms[i].getPtr();
			}
		}
	}
}


AosGenFunc::~AosGenFunc()
{
	OmnDelete [] mConstValues;
	mExprValues = NULL;
	OmnDelete [] mExprValues;
	mExprValues = NULL;
}


// Young, 2015/05/29
AosDataType::E 
AosGenFunc::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	OmnNotImplementedYet;
	return AosDataType::eInvalid;
}


// Chen Ding, 2014/09/16
bool 
AosGenFunc::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	OmnShouldNeverComeHere;
	return false;
}


// Chen Ding, 2014/09/16
bool 
AosGenFunc::syntaxCheck(
		AosRundata *rdata, 
		OmnString &errmsg)
{
	OmnShouldNeverComeHere;
	return false;
}


// Andy Zhang, 2015/08/21
bool
AosGenFunc::setParms(
		AosRundata *rdata,
		AosExprList *parms)
{
	bool rslt = false;
	mSize = parms->size();
	if (mSize > 0)
	{
		OmnDelete [] mConstValues;
		OmnDelete [] mExprValues;

		mConstValues = OmnNew AosValueRslt[mSize];
		mExprValues = OmnNew AosExprObj*[mSize];
		memset(mExprValues, 0, sizeof(AosExprObj*) * mSize);
		for (u32 i = 0; i < mSize; i++)
		{
			mParms.push_back((*parms)[i]->cloneExpr());
			if (mParms[i]->isConstant())
			{
				rslt = mParms[i]->getValue(rdata, 0, mConstValues[i]);
				aos_assert_r(rslt, false);
			}
			else
			{
				mExprValues[i] = mParms[i].getPtr();
			}
		}
	}
	return true;
}

bool 
AosGenFunc::getValue(
		AosRundata *rdata, 
		int	idx,
		AosDataRecordObj *record)
{
	if (mSize == 0)
	{
		mValue.setNull();
		return true;
	}
	return  mParms[idx]->getValue(rdata, record, mValue);
	if (mExprValues[idx])
	{
		return mExprValues[idx]->getValue(rdata, record, mValue);
	}
	mValue = mConstValues[idx];
	return true;
}

