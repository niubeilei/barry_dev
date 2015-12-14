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
#include "JQLExpr/ExprIn.h"
#include "JQLStatement/JqlStmtTable.h"

#include "API/AosApi.h"

AosExprIn::AosExprIn()
:
mField(0),
mValues(0)
{
}

AosExprIn::AosExprIn(
		AosExprObjPtr field, 
		AosExprList *value_list)
:
mFieldPtr(field),
mField(mFieldPtr.getPtr()),
mValuesPtr(value_list)
{
	if (!field)
	{
		OmnAlarm << "ExprIn Field is null." << enderr;
	}
	if (!value_list)
	{
		OmnAlarm << "ExprIn Values is null." << enderr;
	}

	for (u32 i = 0; i < mValuesPtr->size(); i++)
	{
		mValues.push_back((*mValuesPtr)[i].getPtr());
	}
}


AosExprIn::~AosExprIn()
{
}


bool
AosExprIn::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	bool rslt = mField->getValue(rdata, record, value);
	aos_assert_r(rslt, false);
	OmnString vv = value.getStr(); 
	if (vv == "") 
	{
		value.setBool(false);
		return true;
	}

	for (u32 i=0; i<mValues.size(); i++)
	{
		if (!mValues[i]->isConstant())
		{
			OmnAlarm << " AosExprIn value is not counst." << enderr;
			return false;
		}
		if (vv == mValues[i]->getValue(rdata))
		{
			value.setBool(true);
			return true;
		}
	}
	value.setBool(false);
	return true;
}


bool
AosExprIn::getValue(
		AosRundata *rdata,
		const AosValueRslt &key,
		AosValueRslt &value)
{
	OmnString vv = key.getStr(); 
	if (vv == "") 
	{
		value.setBool(false);
		return true;
	}
	for (u32 i=0; i < mValues.size(); i++)
	{
		if (vv == mValues[i]->getValue(rdata))
		{
			value.setBool(true);
			return true;
		}
	}
	value.setBool(false);
	return true;
}

bool                               
AosExprIn::createConds(     
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName)
{                                  
	AosJqlQueryWhereCond *cond = new AosJqlQueryWhereCond;
	OmnString str = ""; 

	for (u32 i=0; i < mValues.size(); i++) {
		str << "eq,";
		str << mValues[i]->getValue(rdata.getPtrNoLock());
		str << ";";
		cond->mValues.push_back((mValues)[i]->getValue(rdata.getPtrNoLock()));
	}

//	cond->mLHS = mField->getValue(rdata.getPtrNoLock());
	cond->mLHS = mField->dumpByNoEscape();
	cond->mRHS = str;
	cond->mOpr = "in";
	//arvin 2015.08.20
	//JIMODB-526
	cond->mExpr = this;
	cond->mLHSExpr = mField;
	conds.push_back(cond);
	return true;
}                                  


AosExprObjPtr
AosExprIn::cloneExpr() const                            
{
	try
	{
		if (mField && mValuesPtr)
		{
			AosExprInPtr expr = OmnNew AosExprIn();
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
		OmnAlarm << "AosExprIn clone is failed" << enderr;
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
AosExprIn::dump() const
{
	OmnString str;
	str << mField->dump();
	str << " IN (";
	for (u32 i=0; i<mValues.size(); i++)
	{
		if (i != 0 )
			str << ",";
		str << mValues[i]->dump();
	}
	str << ")";
	return str;
}

OmnString 
AosExprIn::dumpByNoEscape() const
{
	OmnString str;
	str << mField->dumpByNoEscape();
	str << " IN (";
	for (u32 i=0; i<mValues.size(); i++)
	{
		if (i != 0 )
			str << ",";
		str << mValues[i]->dumpByNoEscape();
	}
	str << ")";
	return str;
}

OmnString
AosExprIn::dumpByStat() const
{
	return dump();
}

