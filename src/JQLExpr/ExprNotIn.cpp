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
// 2014/12/02 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprNotIn.h"
#include "JQLStatement/JqlStmtTable.h"

#include "API/AosApi.h"

AosExprNotIn::AosExprNotIn()
:
mField(0),
mValues(0)
{
}


AosExprNotIn::AosExprNotIn(
		AosExprObjPtr field, 
		AosExprList *value_list)
:
mFieldPtr(field),
mValuesPtr(value_list)
{
	mField = mFieldPtr.getPtr();
	if (!field)
	{
		OmnAlarm << "ExprNotIn Field is null." << enderr;
	}
	if (!value_list)
	{
		OmnAlarm << "ExprNotIn Values is null." << enderr;
	}
	for (u32 i = 0; i < mValues.size(); i++)
	{
		mValues.push_back((*mValuesPtr)[i].getPtr());
	}
}


AosExprNotIn::~AosExprNotIn()
{
}


bool
AosExprNotIn::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	bool rslt = mField->getValue(rdata, record, value);
	aos_assert_r(rslt, false);
	OmnString vv = value.getStr(); 
	//aos_assert_r(vv != "", false);
	for (u32 i=0; i<mValues.size(); i++)
	{
		if (!(mValues)[i]->isConstant())
		{
			OmnAlarm << " AosExprNotIn value is not counst." << enderr;
			return false;
		}
		if (vv == (mValues)[i]->getValue(rdata))
		{
			value.setBool(false);
			return true;
		}
	}
	value.setBool(true);
	return true;
}


bool
AosExprNotIn::getValue(
		AosRundata *rdata,
		const AosValueRslt &key,
		AosValueRslt &value)
{
	OmnString vv = key.getStr(); 
	aos_assert_r(vv != "", false);
	for (u32 i=0; i < mValues.size(); i++)
	{
		if (vv == (mValues)[i]->getValue(rdata))
		{
			value.setBool(false);
			return true;
		}
	}
	value.setBool(true);
	return true;
}


bool                               
AosExprNotIn::createConds(     
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName)
{                                  
	AosJqlQueryWhereCond *cond = new AosJqlQueryWhereCond;
	OmnString str = ""; 

	for (u32 i=0; i < mValues.size(); i++) {
		str << "pf,";
		str << (mValues)[i]->getValue(rdata.getPtrNoLock());
		str << ";";
		cond->mValues.push_back((mValues)[i]->getValue(rdata.getPtrNoLock()));
	}

	cond->mLHS = mField->getValue(rdata.getPtrNoLock());
	cond->mRHS = str;
	cond->mOpr = "not in";
	conds.push_back(cond);
	return true;
}                                  


AosExprObjPtr
AosExprNotIn::cloneExpr() const                            
{
	try
	{
		if (mField && mValuesPtr)
		{
			AosExprNotInPtr expr = OmnNew AosExprNotIn();
			expr->mValuesPtr = new AosExprList;
			for (u32 i=0; i<mValues.size(); i++)
			{
				expr->mValuesPtr->push_back((mValues)[i]->cloneExpr());
				expr->mValues.push_back((*expr->mValuesPtr)[i].getPtr());
			}
			expr->mFieldPtr = mField->cloneExpr();
			expr->mField = expr->mFieldPtr.getPtr();
			return expr; 
		}
		OmnAlarm << "AosExprNotIn clone is failed" << enderr;
		return 0;
	}   

	catch (...)
	{
		OmnAlarm << "failed create expr" << enderr;
		return 0;
	}   

	OmnShouldNeverComeHere;
	return 0;
}


OmnString 
AosExprNotIn::dump() const
{
	OmnString str;
	str << mField->dump();
	str << " NOT IN (";
	for (u32 i=0; i<mValues.size(); i++)
	{
		if (i != 0 )
			str << ",";
		str << (mValues)[i]->dump();
	}
	str << ")";
	return str;
}


OmnString 
AosExprNotIn::dumpByNoEscape() const
{
	OmnString str;
	str << mField->dumpByNoEscape();
	str << " NOT IN (";
	for (u32 i=0; i<mValues.size(); i++)
	{
		if (i != 0 )
			str << ",";
		str << (mValues)[i]->dumpByNoEscape();
	}
	str << ")";
	return str;
}


OmnString
AosExprNotIn::dumpByStat() const
{
	return dump();
}

