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
// Modification History:
// 2014/11/20 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprIsNull.h"
#include "Debug/Except.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"


AosExprIsNull::AosExprIsNull(
		const AosExprObjPtr	&field)
:
mFieldPtr(field),
mField(mFieldPtr.getPtr())
{
	if (!mFieldPtr)
	{
		return;
	}
}


AosExprIsNull::AosExprIsNull(const AosExprIsNull &rhs)
:
mField(0)
{
	if (rhs.mFieldPtr)
	{
		mFieldPtr = rhs.mField->cloneExpr();
		mField = mFieldPtr.getPtr();
	}
}


AosExprIsNull::~AosExprIsNull()
{
}


bool
AosExprIsNull::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value) 
{
	if (!mField)
	{
		AosSetErrorUser(rdata, __func__)
			<< "Field Is NULL" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosValueRslt vv;
	bool rslt= mField->getValue(rdata, record, vv);
	aos_assert_rr(rslt, rdata, false);
	value.setBool(vv.isNull());
	return true;
}

bool
AosExprIsNull::getValue(
		AosRundata *rdata,
		const AosValueRslt &key,
		AosValueRslt &value)
{
	value.setBool(key.isNull());
	return true;
}


AosExprObjPtr
AosExprIsNull::cloneExpr() const                            
{
	return OmnNew AosExprIsNull(*this);
}


int
AosExprIsNull::getNumFieldTerms() const
{
	return 1;
}

	
bool		
AosExprIsNull::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	if (mField) mField->getFields(rdata, fields);
	return true;
}

	
bool
AosExprIsNull::createConds(
		const AosRundataPtr &rdata, 
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName)
{
	AosJqlQueryWhereCond *cond = new AosJqlQueryWhereCond;
	cond->mLHS = mField->getValue(rdata.getPtrNoLock());
	cond->mRHS = "";
	cond->mRHS << '\b';
	cond->mOpr = "eq";
	cond->mExpr = this;
	conds.push_back(cond);
	return true;
}


OmnString 
AosExprIsNull::dump() const
{
	OmnString str;
	str <<  mField->dump() << " IS NULL " ;
	return str;
}


OmnString 
AosExprIsNull::dumpByNoEscape() const
{
	OmnString str;
	str <<  mField->dumpByNoEscape() << " IS NULL " ;
	return str;
}

OmnString 
AosExprIsNull::dumpByStat() const
{
	return dump();
}

