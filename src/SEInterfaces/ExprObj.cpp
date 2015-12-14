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
// 2013/11/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ExprObj.h"

AosExprObjPtr AosExprObj::smExprPtr;
AosExprObj* AosExprObj::smExpr;

OmnString
AosExprObj::getValue(AosRundata *rdata)
{
	AosValueRslt v;
	bool rslt = getValue(rdata, 0, v); 
	aos_assert_r(rslt, "");
	return v.getStr();
}
