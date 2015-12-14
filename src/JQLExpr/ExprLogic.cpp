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
// 2013/09/25 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprLogic.h"

#include "API/AosApi.h"
#include "Alarm/Alarm.h"


AosExprLogic::AosExprLogic(
		AosExprObjPtr lhs, 
		AosExprLogic::Operator opr,
		AosExprObjPtr rhs)
:
AosExprBinary(lhs, rhs),
mOpr(opr)
{
	if (!isValidOpr(opr))
	{
		OmnString errmsg = "Invalid Operator: ";
		errmsg << opr;
		//OmnThrowException(errmsg);
		OmnAlarm << errmsg << enderr;
		return;
	}
}


AosExprLogic::~AosExprLogic()
{
}

bool
AosExprLogic::getValue(
		AosRundata *rdata,
		vector<AosDataRecordObj *> &records,
		AosValueRslt &value)
{
	aos_assert_r(mLHS, false);
	aos_assert_r(mRHS, false);

	AosValueRslt v1;
	AosValueRslt v2;

	if (!mLHS->getValue(rdata, records, v1)) return false;
	if (!mRHS->getValue(rdata, records, v2)) return false;

	if (v1.isNull() || v2.isNull())
	{
		value.setBool(false);  
		return true;
	}
	//aos_assert_rr(v1, rdata, false);
	//aos_assert_rr(v2, rdata, false);

	bool vv1, vv2;
	vv1 = v1.getBool();
	vv2 = v2.getBool();

	switch (mOpr)
	{
	case eAnd:
		 value.setBool(vv1 and vv2);
		 return true;

	case eOr:
		 value.setBool(vv1 or vv2);
		 return true;

	default:
		 AosSetError(rdata, "invalid_operator") << mOpr << enderr;
		 return false;
	}	

	OmnShouldNeverComeHere;
	return true;
}


bool
AosExprLogic::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	aos_assert_r(mLHS, false);
	aos_assert_r(mRHS, false);

	AosValueRslt v1;
	AosValueRslt v2;

	if (!mLHS->getValue(rdata, record, v1)) return false;
	if (!mRHS->getValue(rdata, record, v2)) return false;

	//aos_assert_rr(v1, rdata, false);
	//aos_assert_rr(v2, rdata, false);

	if (v1.isNull() || v2.isNull())
	{
		value.setBool(false);  
		return true;
	}

	bool vv1, vv2;
	vv1 = v1.getBool();
	vv2 = v2.getBool();

	switch (mOpr)
	{
	case eAnd:
		 value.setBool(vv1 and vv2);
		 return true;

	case eOr:
		 value.setBool(vv1 or vv2);
		 return true;

	default:
		 AosSetError(rdata, "invalid_operator") << mOpr << enderr;
		 return false;
	}	

	OmnShouldNeverComeHere;
	return true;
}

bool
AosExprLogic::getValue(
		AosRundata *rdata,
		const AosValueRslt &key,
		AosValueRslt &value)
{
	aos_assert_r(mLHS, false);
	aos_assert_r(mRHS, false);

	AosValueRslt v1;
	AosValueRslt v2;

	if (!mLHS->getValue(rdata, key, v1)) return false;
	if (!mRHS->getValue(rdata, key, v2)) return false;

	if (v1.isNull() || v2.isNull())
	{
		value.setBool(false);  
		return true;
	}

	//aos_assert_rr(v1, rdata, false);
	//aos_assert_rr(v2, rdata, false);

	bool vv1, vv2;
	vv1 = v1.getBool();
	vv2 = v2.getBool();

	switch (mOpr)
	{
	case eAnd:
		 value.setBool(vv1 and vv2);
		 return true;

	case eOr:
		 value.setBool(vv1 or vv2);
		 return true;

	default:
		 AosSetError(rdata, "invalid_operator") << mOpr << enderr;
		 return false;
	}	

	OmnShouldNeverComeHere;
	return true;
}



bool 
AosExprLogic::moveTerms(
		const AosRundataPtr &rdata, 
		bool &moved, 
		AosExprObjPtr &term)
{
	moved = false;
	return true;
}


bool 
AosExprLogic::createConds(              
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName) 
{
	bool rslt = false;
	rslt = mLHS->createConds(rdata, conds,tableName);
	aos_assert_r(rslt, false);
	rslt = mRHS->createConds(rdata, conds,tableName);
	aos_assert_r(rslt, false);
	return true;
}


AosExprObjPtr                                                           
AosExprLogic::cloneExpr() const                                       
{                                                                     
	try                                                               
	{                                                                 
		AosExprObjPtr lhs, rhs;                                        
		lhs = mLHS->cloneExpr();                                      
		aos_assert_r(lhs, 0);                                         
		rhs = mRHS->cloneExpr();                                      
		aos_assert_r(rhs, 0);                                         
		AosExprLogicPtr expr = OmnNew AosExprLogic(lhs, mOpr, rhs);     
		return expr;                                                  
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
AosExprLogic::dump() const
{
	OmnString str = "";

	if (mOpr == eAnd)
		str << mLHS->dump() << " and " << mRHS->dump(); 
	else 
		str << mLHS->dump() << " or " << mRHS->dump(); 

	return str;
}

OmnString
AosExprLogic::dumpByNoEscape() const
{
	OmnString str = "";

	if (mOpr == eAnd)
		str << mLHS->dumpByNoEscape() << " and " << mRHS->dumpByNoEscape(); 
	else 
		str << mLHS->dumpByNoEscape() << " or " << mRHS->dumpByNoEscape(); 

	return str;
}

OmnString
AosExprLogic::dumpByStat() const
{
	OmnString str = "";

	if (mOpr == eAnd)
		str << mLHS->dumpByStat() << " and " << mRHS->dumpByStat(); 
	else 
		str << mLHS->dumpByStat() << " or " << mRHS->dumpByStat(); 

	return str;
}

OmnString 
AosExprLogic::getEnumStr()
{
	if (mOpr == eAnd)
		return "and";
	if (mOpr == eOr)
		return "or";
	return "";
}

bool 
AosExprLogic::isHasOr() const
{
	if (mOpr == eOr) return true;
	if (mLHS->isHasOr() || mRHS->isHasOr())
		return true;
	return false;
}
