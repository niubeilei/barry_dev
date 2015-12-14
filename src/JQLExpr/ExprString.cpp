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
#include "JQLExpr/ExprString.h"
#include "API/AosApi.h"

AosExprString::AosExprString()
{
}


AosExprString::AosExprString(const char *value)
{
	mValue = value;
}


AosExprString::~AosExprString()
{
}


bool
AosExprString::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
//	if (mValue.length() <= 0) return false;
	value.setStr(mValue);
	return true;
}

bool
AosExprString::getValue(
		AosRundata *rdata,
		vector<AosDataRecordObj *> &records,
		AosValueRslt &value)
{
//	if (mValue.length() <= 0) return false;
	value.setStr(mValue);
	return true;
}

OmnString 
AosExprString::dump() const
{
	OmnString str;
	str << "\'" << mValue << "\'";
	return str;
} 


OmnString 
AosExprString::dumpByNoEscape() const
{
	return dump();
} 

OmnString 
AosExprString::dumpByStat() const
{
	return dump();
} 

OmnString 
AosExprString::dumpByNoQuote()const
{
	return mValue;
}

AosExprObjPtr
AosExprString::cloneExpr() const                            
{
	try
	{
		AosExprStringPtr expr = OmnNew AosExprString();
		expr->mValue = mValue;
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


AosJimoPtr
AosExprString::cloneJimo() const
{
	return OmnNew AosExprString(*this);
}


