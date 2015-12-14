////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 2013/10/08 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprSearchCase.h"
#include "Debug/Except.h"

#include "WordParser/WordParser.h"
#include "WordParser/WordNorm.h"     

#include "API/AosApi.h"
#include "Util/OmnNew.h"



AosExprSearchCase::AosExprSearchCase(const AosExprSimpleCasePtr &simple) 
{
	mExprs = simple->getExprs();
}


AosExprSearchCase::AosExprSearchCase(const AosExprSearchCase &rhs)
{
	mExprs.clear();
	for (size_t i=0; i<rhs.mExprs.size(); i++)
	{
		mExprs.push_back(rhs.mExprs[i]->clone());
	}
}


AosExprSearchCase::~AosExprSearchCase()
{
}


bool
AosExprSearchCase::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	// The grammar is:
	// 	case when expr1 then expr2
	// 	case when expr1 then expr2
	// 	....
	// 	case when expr1 then expr2
	// That is, if expr1 is true, it returns the value of expr2. 

	AosValueRslt vv;
	bool rslt;
	for (u32 i=0; i<mExprs.size(); i++)
	{
		rslt = mExprs[i]->mWhenExpr->getValue(rdata, record, vv);
		aos_assert_r(rslt, false);
		if (vv.getBool())
		{
			return mExprs[i]->mThenExpr->getValue(rdata, record, value);
		}
	}
	value.setNull();
	return true;
}


AosExprObjPtr
AosExprSearchCase::cloneExpr() const                            
{
	return OmnNew AosExprSearchCase(*this);
}


bool		
AosExprSearchCase::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	bool rslt;

	if(mExprs.size()) 
	{
		for (u32 i=0; i<mExprs.size(); i++)
		{
			rslt = mExprs[i]->mWhenExpr->getFields(rdata, fields);
			aos_assert_r(rslt, false);
			rslt = mExprs[i]->mThenExpr->getFields(rdata, fields);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}


OmnString 
AosExprSearchCase::dump() const
{
	OmnString str, str1, str2;

	for (u32 i = 0; i < mExprs.size(); i++)
	{
		str1 = mExprs[i]->mWhenExpr->dump();
		str2 = mExprs[i]->mThenExpr->dump();
		str << " when " << str1 << " then " << str2 << " ";
	}
	return str;
}


OmnString 
AosExprSearchCase::dumpByNoEscape() const
{
	OmnString str, str1, str2;

	for (u32 i = 0; i < mExprs.size(); i++)
	{
		str1 = mExprs[i]->mWhenExpr->dumpByNoEscape();
		str2 = mExprs[i]->mThenExpr->dumpByNoEscape();
		str << " when " << str1 << " then " << str2 << " ";
	}
	return str;
}


bool		
AosExprSearchCase::getFieldsByStat(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	bool rslt;

	if(mExprs.size()) 
	{
		for (u32 i=0; i<mExprs.size(); i++)
		{
			rslt = mExprs[i]->mWhenExpr->getFieldsByStat(rdata, fields);
			aos_assert_r(rslt, false);
			rslt = mExprs[i]->mThenExpr->getFieldsByStat(rdata, fields);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}


OmnString 
AosExprSearchCase::dumpByStat() const
{
	OmnString str, str1, str2;

	for (u32 i = 0; i < mExprs.size(); i++)
	{
		str1 = mExprs[i]->mWhenExpr->dumpByStat();
		str2 = mExprs[i]->mThenExpr->dumpByStat();
		str << " when " << str1 << " then " << str2 << " ";
	}
	return str;
}

