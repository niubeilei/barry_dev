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
#include "JQLExpr/ExprUserVar.h"
#include "JQLParser/JQLParser.h"
#include "API/AosApi.h"

extern AosJQLParser gAosJQLParser;  
AosExprUserVar::AosExprUserVar()
{
}


AosExprUserVar::AosExprUserVar(const AosExprUserVar &rhs)
:
mName(rhs.mName),
mValue(rhs.mValue),
mFieldTypeInfo(rhs.mFieldTypeInfo)
{
}


AosExprUserVar::~AosExprUserVar()
{
}


AosDataType::E 
AosExprUserVar::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	aos_assert_r(mFieldTypeInfo, AosDataType::eInvalid);	
	OmnString type = mFieldTypeInfo->getType();

	return AosDataType::toEnum(type);
}


bool 
AosExprUserVar::getValue(
	AosRundata *rdata,
	AosDataRecordObj *record,
	AosValueRslt &value)
{
	if (mName == "")
	{
		AosSetErrorUser(rdata, __func__)
			<< "The uservar name is NULL." << enderr;
		OmnAlarm << enderr;
		return NULL;
	}

	AosExprObjPtr expr = gAosJQLParser.getUserVar(mName);
	AosExprUserVarPtr var = dynamic_cast<AosExprUserVar*>(expr.getPtr());
	if (!var)
	{
		AosSetErrorUser(rdata, __func__)
			<< "The uservar is undefined. Name is " << mName  << enderr;
		OmnAlarm << enderr;
		return NULL;
	}
	return var->getValue(record, value, rdata);
}


OmnString 	
AosExprUserVar::dump() const
{
	OmnString vv;
	vv << "@`" << mName <<"`";
	return vv;
}

OmnString 	
AosExprUserVar::dumpByNoEscape() const 
{
	OmnString vv;
	vv << "@" << mName;
	return vv;
}

OmnString 	
AosExprUserVar::dumpByStat() const 
{
	return dumpByNoEscape();
}

AosExprObjPtr 
AosExprUserVar::cloneExpr() const
{
	return new AosExprUserVar(*this);
}


void 
AosExprUserVar::setType(const AosJQLDataFieldTypeInfoPtr &type)
{
	mFieldTypeInfo = type;
}


void 
AosExprUserVar::setName(const OmnString &name)
{
	mName = name;
}

void
AosExprUserVar::setValue(const AosValueRslt &value)
{
	mValue = value;
}


bool 
AosExprUserVar::getValue(
	AosDataRecordObj *record,
	AosValueRslt &value,
	AosRundata *rdata)
{
	value = mValue;
	return true;
}

