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
// 2014/04/08 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprNameValue.h"

#include "API/AosApi.h"


AosExprNameValue::AosExprNameValue(const OmnString &name, AosExprObjPtr value)
:
mName(name),
mValue(value)
{
}


bool
AosExprNameValue::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	// Chen Ding, 2015/05/30, #3.1-104
	// value.setValue(mValue);
	if (!mValue)
	{
		value.setNull();
		return true;
	}
	return mValue->getValue(rdata, record, value);
}

OmnString
AosExprNameValue::dump() const
{
	OmnString str;
	str << "`" <<mName << "`" << " : " << mValue->dump();
	return str;
}

OmnString
AosExprNameValue::dumpByNoEscape() const
{
	OmnString str;
	str << mName << " : " << mValue->dumpByNoEscape();
	return str;
}

OmnString
AosExprNameValue::dumpByStat() const
{
	return dump();
}



AosExprObjPtr
AosExprNameValue::cloneExpr() const                            
{
	try
	{
		AosExprNameValuePtr expr = OmnNew AosExprNameValue();
		expr->mName = mName;
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


void 
AosExprNameValue::setNameValue(const OmnString &name, const AosExprObjPtr &value)
{
	mName = name;
	mValue = value;
}


AosJimoPtr
AosExprNameValue::cloneJimo() const
{
	return OmnNew AosExprNameValue(*this);
}

OmnString 
AosExprNameValue::dumpByNoQuote()const
{   
	    
	    return mValue->dumpByNoQuote();
}                                         

vector<AosExprObjPtr>                                           
AosExprNameValue::getExprList()
{
	vector<AosExprObjPtr> exprs;
	if (!mValue) 
	{
		OmnAlarm << "value_is_null" << enderr;
		return exprs;
	}
	return mValue->getExprList();
}
