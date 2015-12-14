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
#include "JQLExpr/ExprArith.h"
#include "JQLExpr/ExprNumber.h"
#include "JQLStatement/JqlStatement.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include <float.h>


AosExprArith::AosExprArith(
		AosExprObjPtr lhs, 
		AosExprArith::Operator opr,
		AosExprObjPtr rhs)
:
AosExprBinary(lhs, rhs),
mOpr(opr)
{
	if (!isValidOpr(opr))
	{
		OmnString errmsg = "Invalid Operator: ";
		errmsg << opr;
		OmnThrowException(errmsg);
		return;
	}
}


AosExprArith::~AosExprArith()
{
}


AosDataType::E 
AosExprArith::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	aos_assert_r(mLHS && mRHS, AosDataType::eInvalid);
	AosDataType::E l_type = mLHS->getDataType(rdata, record);
	AosDataType::E r_type = mRHS->getDataType(rdata, record);

	return AosDataType::autoTypeConvert(l_type, r_type);
}


bool
AosExprArith::getValue(
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

	if (v1.isNull() || v2.isNull()) 
	{
		value.setNull();
		return true;
	}

	AosDataType::E type = AosDataType::autoTypeConvert(v1.getType(), v2.getType());

	switch (mOpr)
	{
	case eAdd:
		 value = AosValueRslt::doArith(ArithOpr::eAdd, type, v1, v2);
		 return true;

	case eMinus:
		 value = AosValueRslt::doArith(ArithOpr::eSub, type, v1, v2);
		 return true;

	case eDivide:
		 value = AosValueRslt::doArith(ArithOpr::eDiv, type, v1, v2);
		 return true;

	case eMultiply:
		 value = AosValueRslt::doArith(ArithOpr::eMul, type, v1, v2);
		 return true;

	case eMod:
		 value = AosValueRslt::doArith(ArithOpr::eMod, type, v1, v2);
		 //value = v1 % v2; //fpei: may have problem
		 return true;
	default:
		 AosSetError(rdata, "invalid_operator") << mOpr << enderr;
		 return false;
	}	

	OmnShouldNeverComeHere;
	return true;
}


bool
AosExprArith::mergeTerms(
		const AosRundataPtr &rdata, 
		bool &merged, 
		AosExprObjPtr &merged_term)
{
	return true;
}


AosExprObjPtr                                                                     
AosExprArith::cloneExpr() const                                                 
{                                                                               
	try                                                                         
	{                                                                           
		AosExprObjPtr lhs, rhs;                                                  
		aos_assert_r(mLHSPtr, NULL);
		lhs = mLHS->cloneExpr();                                                
		aos_assert_r(lhs, 0);                                                  
		aos_assert_r(mRHSPtr, NULL);
		rhs = mRHS->cloneExpr();                                                
		aos_assert_r(rhs, 0);                                                  
		AosExprArithPtr expr = OmnNew AosExprArith(lhs, mOpr, rhs);               
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
AosExprArith::dump() const 
{
	OmnString str;
	str << mLHS->dump();
	str << getEnumStr();
	str << mRHS->dump();
	return str;
}

OmnString 
AosExprArith::dumpByNoEscape() const 
{
	OmnString str;
	str << mLHS->dumpByNoEscape();
	str << getEnumStr();
	str << mRHS->dumpByNoEscape();
	return str;
}

OmnString 
AosExprArith::dumpByStat() const 
{
	OmnString str;
	str << mLHS->dumpByStat();
	str << getEnumStr();
	str << mRHS->dumpByStat();
	return str;
}


OmnString
AosExprArith::getEnumStr() const 
{
	if (mOpr == eAdd) return "+";
	if (mOpr == eMinus) return "-";
	if (mOpr == eMultiply) return "*";
	if (mOpr == eDivide) return "/";
	if (mOpr == eMod) return "%";
	return "";
}

