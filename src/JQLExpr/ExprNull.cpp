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
// 2014/11/28 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprNull.h"
#include "API/AosApi.h"

AosExprNull::AosExprNull()
{
}


bool
AosExprNull::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	value.setStr("__NULL__");
	return true;
}

OmnString
AosExprNull::dump() const 
{
	return "NULL";
}

OmnString 	
AosExprNull::dumpByNoEscape() const
{
	return dump();
}

OmnString 
AosExprNull::dumpByStat() const
{
	return dump();
};



AosExprObjPtr
AosExprNull::cloneExpr() const                            
{
	try
	{
		return OmnNew AosExprNull();
	}   

	catch (...)
	{
		OmnAlarm << "failed create expr" << enderr;
		return 0;
	}   

	OmnShouldNeverComeHere;
	return 0;
}

