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
#ifndef AOS_JQLStatement_JqlQueryWhereCond_H
#define AOS_JQLStatement_JqlQueryWhereCond_H

#include "JQLStatement/JqlDataStruct.h"
#include "JQLStatement/Ptrs.h"

#include "SEInterfaces/ExprObj.h"
#include "Rundata/Rundata.h"
#include "alarm_c/alarm.h"
#include "Util/String.h"
#include "Debug/Debug.h"
#include "Util/Opr.h"
#include <vector>
using namespace std;

class AosJqlStatement;
class AosJqlQueryWhereCond : public AosJqlDataStruct
{
public:
	struct QueryFilter{
		OmnString mType;
		int mFieldIdx;
		OmnString mFieldName;
		OmnString mOpr;
		OmnString mValue;
		OmnString mValue2;
		OmnString mCondStr;

		QueryFilter(){
			mFieldIdx = -1;
			mType = "keyfield";
		}

		void setData(int idx,
				OmnString opr,
				OmnString fieldname,
				OmnString value,
				OmnString value2 = "",
				OmnString cond_str = "")
		{
			if (idx >= 0 )
				mFieldIdx = idx;
			mOpr = opr;
			mFieldName = fieldname;
			mValue = value;
			mValue2 = value2;
			mCondStr = cond_str;
		}
	};

public:
	OmnString			mLHS;
	OmnString			mRHS;
	OmnString			mValue2;
	AosValueRslt		mValue2Rslt;
	OmnString			mOpr;
	OmnString			mIILName;
	OmnString			mCMPIILName;
	OmnString			mCondStr;
	OmnString 			mTableName;
	vector<OmnString>	mValues;
	bool 				mIsIn;
	bool				mIsJoin;
	bool				mIsUsed;
	bool				mIsOrder;
	bool				mIsAsc;
	bool				mIsMegre;
	vector<QueryFilter> mQueryFilters;
	AosExprObjPtr		mExpr;
	AosExprObjPtr		mLHSExpr;
	AosExprObjPtr		mRHSExpr;

public:
	AosJqlQueryWhereCond();
	~AosJqlQueryWhereCond();

	bool addQueryFilter(QueryFilter &query_filter);
	bool addQueryFilters(vector<QueryFilter> &query_filters);
	bool addQueryFilter(
			const AosRundataPtr &rdata,
			const AosJqlQueryWhereCondPtr &cond,
			const u32 &idx);

	bool setData(AosJqlQueryWhereCondPtr cond);

	bool isRange() {
		if (mOpr != "eq" && mOpr != "in") return true;
		return false;
	}

	bool isNeedCastOpr()
	{
		if (mOpr == "like" 
			|| mOpr == "not like"
			|| mOpr == "not in" 
			|| mOpr == "an") return true;
		return false;
	}

	bool isNotAllowedUseIIL(){
		if ( mOpr == "like" 
				|| mOpr == "not like"
				|| mOpr == "not in" 
				|| mOpr == "an") return true;
		return false;
	}

	void setOrder(const AosJqlQueryWhereCondPtr &cond) {
		if (cond->mIsOrder) mIsOrder = true;
		if (cond->mIsAsc) mIsAsc = true;
	}

	OmnString getLeftValue() { return mLHS;}
	OmnString getRightValue() { return mRHS;}
	OmnString getOpr() { return mOpr;}
	OmnString getIILName() { return mIILName;}
	bool isJoinCond(){return mIsJoin;}
	OmnString toString(const AosRundataPtr &rdata);
	bool isNumber(const AosJqlColumnPtr &column);
	AosJqlQueryWhereCondPtr clone() const;
	
	OmnString getFieldName(AosRundataPtr rdata)
	{
		return (*mLHSExpr->getParmList())[0]->getValue(rdata.getPtr());
	}
	void getFields(AosRundataPtr rdata,vector<AosJqlSelectFieldPtr>& fields)
	{
		mLHSExpr->getFieldsByStat(rdata,fields);
	}
};

#endif
