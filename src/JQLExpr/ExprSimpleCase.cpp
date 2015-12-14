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
// A case simple expression is:
// The grammar is:
// case expr
// 		when expr then expr
// 		when expr then expr
// 		...
// 		when expr then expr
//
// Example:
// 	case sex
// 		when '1' then 'male'
// 		when '2' then 'female'
// 	
// Modification History:
// 2013/10/08 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprSimpleCase.h"

#include "Util/Opr.h"


AosExprSimpleCase::AosExprSimpleCase()
{
}


AosExprSimpleCase::AosExprSimpleCase(const AosExprSimpleCasePtr &rhs)
{
	if (rhs->mExpr)
	{
		mExpr = rhs->mExpr->cloneExpr();
	}

	if(rhs->mExprs.size())
	{
		mExprs = rhs->mExprs;
	}
}


AosExprSimpleCase::~AosExprSimpleCase()
{
}


bool
AosExprSimpleCase::setCaseExpr(const AosExprObjPtr &expr)
{
	if (!expr)
	{
		AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
		AosSetErrorUser(rdata, __func__)
			<< "Case expression empty!" << enderr;                 
		OmnAlarm << __func__ << enderr;
		return false;
	}

	mExpr = expr;
	return true;
}


bool
AosExprSimpleCase::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value) 
{
	AosValueRslt lhs;
	aos_assert_rr(mExpr, rdata, false);
	bool rslt = mExpr->getValue(rdata, record, lhs);
	aos_assert_rr(rslt, rdata, false);
	if (lhs.isNull())
	{
		value.setNull();
		return true;
	}

	for (u32 i=0; i<mExprs.size(); i++)
	{
		AosValueRslt rhs;
		rslt = mExprs[i]->mWhenExpr->getValue(rdata, record, rhs);
		aos_assert_rr(rslt, rdata, false);
		if (!rhs.isNull())
		{
			AosDataType::E type = AosDataType::autoTypeConvert(lhs.getType(), rhs.getType());
			if (AosValueRslt::doComparison(eAosOpr_eq, type, lhs, rhs))
			{
				rslt = mExprs[i]->mThenExpr->getValue(rdata, record, value);
				if (!rslt) return false;
				return true;
			}
		}
	}

	value.setNull();
	return true;
}


/*
bool
AosExprSimpleCase::appendPair(
		const AosExprObjPtr &expr1, 
		const AosExprObjPtr &expr2)
{
	mExprs.push_back(expr1);
	mExprs.push_back(expr2);
	return true;
}
*/


bool 
AosExprSimpleCase::appendWhenCase(AosJqlWhenCaseExpr * whencase)
{
	if (whencase) mExprs.push_back(whencase);
	return true;
}


AosExprObjPtr
AosExprSimpleCase::cloneExpr() const                            
{
	return OmnNew AosExprSimpleCase(*this);
}


bool		
AosExprSimpleCase::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	bool rslt;
	if (mExpr) mExpr->getFields(rdata, fields);
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


bool		
AosExprSimpleCase::getFieldsByStat(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	bool rslt;
	if (mExpr) mExpr->getFieldsByStat(rdata, fields);
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


bool
AosExprSimpleCase::createConds(
		const AosRundataPtr &rdata, 
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName)
{
	OmnShouldNeverComeHere;
	return false;
}


OmnString 
AosExprSimpleCase::dump() const
{
	OmnString str, str1, str2;
	str << mExpr->dump();
	aos_assert_r(mExprs.size()>0, "");

	for (u32 i = 0; i < mExprs.size(); i++)
	{
		str1 = mExprs[i]->mWhenExpr->dump();
		str2 = mExprs[i]->mThenExpr->dump();
		str << " when " << str1 << " then " << str2 << " ";
	}
	return str;
}

OmnString 
AosExprSimpleCase::dumpByNoEscape() const
{
	OmnString str, str1, str2;
	str << mExpr->dumpByNoEscape();
	aos_assert_r(mExprs.size()>0, "");

	for (u32 i = 0; i < mExprs.size(); i++)
	{
		str1 = mExprs[i]->mWhenExpr->dumpByNoEscape();
		str2 = mExprs[i]->mThenExpr->dumpByNoEscape();
		str << " when " << str1 << " then " << str2 << " ";
	}
	return str;
}


OmnString 
AosExprSimpleCase::dumpByStat() const
{
	OmnString str, str1, str2;
	str << mExpr->dumpByStat();
	aos_assert_r(mExprs.size()>0, "");

	for (u32 i = 0; i < mExprs.size(); i++)
	{
		str1 = mExprs[i]->mWhenExpr->dumpByStat();
		str2 = mExprs[i]->mThenExpr->dumpByStat();
		str << " when " << str1 << " then " << str2 << " ";
	}
	return str;
}

