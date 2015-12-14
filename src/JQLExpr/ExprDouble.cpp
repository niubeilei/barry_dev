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
#include "JQLExpr/ExprDouble.h"

#include "API/AosApi.h"


AosExprDouble::AosExprDouble(double number)
{
	mNumber = number;
}

bool
AosExprDouble::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	value.setDouble(mNumber);
	return true;
}


AosExprObjPtr
AosExprDouble::cloneExpr() const                            
{
	try
	{
		AosExprDoublePtr expr = OmnNew AosExprDouble();
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
AosExprDouble::dump() const
{
	OmnString str;
	str << mNumber;
	return str;
}


OmnString 
AosExprDouble::dumpByNoEscape() const
{
	return dump();
}

OmnString 
AosExprDouble::dumpByStat() const
{
	return dump();
}
