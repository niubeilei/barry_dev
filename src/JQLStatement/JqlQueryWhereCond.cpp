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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlQueryWhereCond.h"

#include "alarm_c/alarm.h"
#include "JQLStatement/JqlStatement.h"
#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "Debug/Debug.h"
#include "Util/Opr.h"
#include <vector>

AosJqlQueryWhereCond::AosJqlQueryWhereCond()
{
	mIsMegre = false;
	mIsJoin = false;
	mIsIn = false;
	mIsUsed = false;
	mIsOrder = false;
	mIsAsc = true;
	mExpr = NULL;
}

AosJqlQueryWhereCond::~AosJqlQueryWhereCond()
{
}


bool 
AosJqlQueryWhereCond::addQueryFilters(vector<QueryFilter> &query_filters)
{
	for (u32 i = 0; i < query_filters.size(); i++) {
		mQueryFilters.push_back(query_filters[i]);
	}
	return true;
}

bool 
AosJqlQueryWhereCond::addQueryFilter(QueryFilter &query_filter)
{
	mQueryFilters.push_back(query_filter);
	return true;
}

bool 
AosJqlQueryWhereCond::addQueryFilter(
		const AosRundataPtr &rdata,
		const AosJqlQueryWhereCondPtr &cond,
		const u32 &idx)
{
	if (mOpr == "") mOpr = "an";
	if (mRHS == "") mRHS = "*";

	setOrder(cond);
	OmnString cond_str;
	QueryFilter query_filter;
	if (cond->mOpr != "an") {
		query_filter.setData(idx, "an", cond->mLHS, "*", "", cond->toString(rdata));
		addQueryFilter(query_filter);
	}
	mQueryFilters.insert(mQueryFilters.end(), cond->mQueryFilters.begin(), cond->mQueryFilters.end());
	return true;
}

bool 
AosJqlQueryWhereCond::setData(AosJqlQueryWhereCondPtr cond)
{
	if (cond->mLHS != "") mLHS = cond->mLHS;
	if (cond->mRHS != "") mRHS = cond->mRHS;
	if (cond->mValue2 != "") mValue2 = cond->mValue2;
	if (cond->mOpr != "") mOpr = cond->mOpr;
	if (cond->mIILName != "") mIILName = cond->mIILName;
	if (cond->mCMPIILName != "") mCMPIILName = cond->mCMPIILName;
	if (cond->mCondStr != "") mCondStr= cond->mCondStr;
	if (cond->mTableName != "") mTableName = cond->mTableName;
	if (cond->mIsIn) mIsIn = true;
	if (cond->mIsJoin) mIsJoin = true;
	if (cond->mIsUsed) mIsUsed = true;
	if (cond->mIsOrder) mIsOrder = true;
	if (cond->mIsAsc) mIsAsc = true;
	if (cond->mExpr) mExpr = cond->mExpr->cloneExpr();
	if (cond->mLHSExpr) mLHSExpr = cond->mLHSExpr->cloneExpr();
	if (cond->mRHSExpr) mRHSExpr = cond->mRHSExpr->cloneExpr();

	mValues.insert(mValues.end(), cond->mValues.begin(), cond->mValues.end());
	addQueryFilters(cond->mQueryFilters);
	return true;
}

OmnString 
AosJqlQueryWhereCond::toString(const AosRundataPtr &rdata)
{
	AosJqlColumnPtr column = 0; 
	if (mTableName !="")
		column = AosJqlStatement::getTableField(rdata, mTableName, mLHS);
	OmnString rhs;
	rhs << "\"" << mRHS << "\"";
	if (mRHSExpr) rhs = mRHSExpr->dumpByNoEscape();


	if (mOpr == "an")
	{
		OmnString str = "";
		for (u32 i = 0; i < mQueryFilters.size(); i++)
		{
			i == 0 
				? str << mQueryFilters[i].mCondStr
				: str << " and " << mQueryFilters[i].mCondStr;
		}
		return str;
	}

	if (mExpr)
		return mExpr->dump();

	OmnString str;
	if (mOpr == "in" || mOpr == "not in")
	{ 
		str << mLHS << " " << mOpr <<" (";
		for (u32 i = 0; i < mValues.size(); i++)
		{
			if (i > 0) str << ",";
			if (isNumber(column))
				str << "" << mValues [i] << "";
			else
				str << "\"" << mValues [i] << "\"";
		}
		str << ")";
		return str;
	}
	if (mOpr == "r1") {
		if (isNumber(column))
			str << mLHS  << ">= " << mRHS << " and " << mLHS << " <= " << mValue2 << "";
		else
			str << mLHS  << ">= \"" << mRHS << "\" and " << mLHS << " <= \"" << mValue2 << "\"";
		return str;
	}

	if (mOpr == "r2") {
		if (isNumber(column))
			str << mLHS  << "<= " << mRHS << " and " << mLHS << " < " << mValue2 << "";
		else
			str << mLHS  << "<= \"" << mRHS << "\" and " << mLHS << " < \"" << mValue2 << "\"";
		return str;
	}

	if (mOpr == "r3") {
		if (isNumber(column))
			str << mLHS  << "> " << mRHS << " and " << mLHS << " <= " << mValue2 << "";
		else
			str << mLHS  << "> \"" << mRHS << "\" and " << mLHS << " <= \"" << mValue2 << "\"";
		return str;
	}

	if (mOpr == "r4") {
		if (isNumber(column))
			str << mLHS  << "> " << mRHS << " and " << mLHS << " < " << mValue2 << "";
		else
			str << mLHS  << "> \"" << mRHS << "\" and " << mLHS << " < \"" << mValue2 << "\"";
		return str;
	}

	str << mLHS << " " << AosOpr_toStr2(mOpr) << " " << rhs << "";
	return str;
}

bool 
AosJqlQueryWhereCond::isNumber(const AosJqlColumnPtr &column)
{
	if (!column)
		return false;
	OmnString field_type;
	field_type = AosDataType::getTypeStr(column->type);       
	field_type = AosJqlStatement::AosJType2DType(field_type);
	if (field_type == "number") return true; 
	//andy 2015.08.13
	//JIMODB-313
	if (field_type == "double") return true; 
	if (field_type == "u64") return true; 
	return false;
}

AosJqlQueryWhereCondPtr
AosJqlQueryWhereCond::clone() const
{
	AosJqlQueryWhereCond *v = new AosJqlQueryWhereCond;
	v->mLHS = mLHS;      
	v->mRHS = mRHS;      
	v->mValue2 = mValue2;      
	v->mOpr = mOpr;      
	v->mIILName = mIILName;  
	v->mCMPIILName = mCMPIILName;  
	v->mTableName = mTableName;
	v->mCondStr = mCondStr;  
	v->mValues.clear();
	v->mValues.insert(v->mValues.end(), mValues.begin(), mValues.end());

	v->mIsIn = mIsIn;
	v->mIsJoin = mIsJoin;
	v->mIsUsed = mIsUsed;
	v->mIsOrder = mIsOrder;
	v->mIsAsc = mIsAsc;
	v->mExpr = mExpr;
	v->mLHSExpr = mLHSExpr;
	v->mRHSExpr = mRHSExpr;

	v->mQueryFilters.clear();
	v->mQueryFilters.insert(v->mQueryFilters.end(), mQueryFilters.begin(), mQueryFilters.end());
	return v;
}
