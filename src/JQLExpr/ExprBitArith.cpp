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
#include "JQLExpr/ExprBitArith.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"


AosExprBitArith::AosExprBitArith(
		AosExprObjPtr lhs, 
		AosExprBitArith::Operator opr,
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


AosExprBitArith::~AosExprBitArith()
{
}


bool
AosExprBitArith::getValue(
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

	u32 vv1, vv2;
	vv1 = v1.getU64();
	vv2 = v2.getU64();

	switch (mOpr)
	{
	case eBitAnd:
		 value.setU64(vv1 & vv2);
		 return true;

	case eBitOr:
		 value.setU64(vv1 | vv2);
		 return true;

	case eBitXor:
		 value.setU64(vv1 ^ vv2);
		 return true;

	default:
		 AosSetError(rdata, "invalid_operator") << mOpr << enderr;
		 return false;
	}	

	OmnShouldNeverComeHere;
	return true;
}


AosExprObjPtr                                                           
AosExprBitArith::cloneExpr() const                                       
{                                                                     
    try                                                               
    {                                                                 
        AosExprObjPtr lhs, rhs;                                        
        lhs = mLHS->cloneExpr();                                      
        aos_assert_r(lhs, 0);                                         
        rhs = mRHS->cloneExpr();                                      
        aos_assert_r(rhs, 0);                                         
        AosExprBitArith *expr = OmnNew AosExprBitArith(lhs, mOpr, rhs);     
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
