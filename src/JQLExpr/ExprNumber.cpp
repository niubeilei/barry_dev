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
#include "JQLExpr/ExprNumber.h"

#include "API/AosApi.h"


AosExprNumber::AosExprNumber(const int64_t number)
{
	mNumber = number;
}

bool
AosExprNumber::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value) 
{
	value.setI64(mNumber);
	return true;
}

bool
AosExprNumber::getValue(
		AosRundata *rdata,
		vector<AosDataRecordObj *> &records,
		AosValueRslt &value)
{
	value.setI64(mNumber);
	return true;
}

AosExprObjPtr
AosExprNumber::cloneExpr() const                            
{
	try
	{
		AosExprNumberPtr expr = OmnNew AosExprNumber();
		expr->mNumber = mNumber;
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
AosExprNumber::dump() const
{
	OmnString str;
	str << mNumber;
	return str;
}

OmnString
AosExprNumber::dumpByNoEscape() const
{
	return dump();
}

OmnString
AosExprNumber::dumpByStat() const
{
	return dump();
}
