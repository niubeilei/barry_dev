////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/10/26	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/GenericObj.h"
#include "Rundata/Rundata.h"

AosGenericObj::AosGenericObj()
{
}


AosGenericObj::AosGenericObj(const int version)
:
AosJimo(AosJimoType::eGenericObj, version)
{
}


AosGenericObj::AosGenericObj(const AosGenericObj &rhs)
:
AosJimo(rhs)
{
	//mKeywords = rhs.mKeywords;
}


AosGenericObj::~AosGenericObj()
{
}


bool 
AosGenericObj::execute(
	AosRundata *rdata,
	const OmnString &verb_name,
	const OmnString &obj_name,
	const OmnString &json,
	const AosJimoProgObjPtr &prog)
{
	if (verb_name == AOSGENOBJ_METHOD_CREATE)
		return createByJql(rdata, obj_name, json, prog);
	if (verb_name == AOSGENOBJ_METHOD_RUN || verb_name == AOSGENOBJ_METHOD_START || verb_name == AOSGENOBJ_METHOD_FEED)
		return runByJql(rdata, obj_name, json);
	if (verb_name == AOSGENOBJ_METHOD_SHOW)
		return showByJql(rdata, obj_name, json);
	if (verb_name == AOSGENOBJ_METHOD_DROP)
		return dropByJql(rdata, obj_name, json);
	if (verb_name == AOSGENOBJ_METHOD_STOP)
		return stopByJql(rdata, obj_name, json);

	OmnString err;
	err << verb_name << " is not a generic methond.";

	AosSetErrorUser(rdata,  __func__) << err << enderr;
	OmnAlarm << __func__ << enderr;
	return false;
}


OmnString 
AosGenericObj::getObjType(AosRundata *rdata)
{
	AosSetErrorUser(rdata,  __func__) 
		<< "The GenericObj Is Not Implemented." << enderr;
	return "";
}


bool 
AosGenericObj::createByJql(
	AosRundata *rdata,
	const OmnString &objname, 
	const OmnString &json, 
	const AosJimoProgObjPtr &prog)
{
	AosSetErrorUser(rdata,  __func__) 
		<< "The GenericObj Is Not Implemented." << enderr;
	return false;
}


bool
AosGenericObj::runByJql(
	AosRundata *rdata,
	const OmnString &objname, 
	const OmnString &json)
{
	AosSetErrorUser(rdata,  __func__) 
		<< "The GenericObj Is Not Implemented." << enderr;
	return false;
}


bool 
AosGenericObj::showByJql(
	AosRundata *rdata,
	const OmnString &obj_name, 
	const OmnString &json)
{
	AosSetErrorUser(rdata,  __func__) 
		<< "The GenericObj Is Not Implemented." << enderr;
	return false;
}

bool 
AosGenericObj::stopByJql(
	AosRundata *rdata,
	const OmnString &obj_name, 
	const OmnString &json)
{
	AosSetErrorUser(rdata,  __func__) 
		<< "The GenericObj Is Not Implemented." << enderr;
	return false;
}

bool 
AosGenericObj::dropByJql(
	AosRundata *rdata,
	const OmnString &obj_name, 
	const OmnString &json)
{
	AosSetErrorUser(rdata,  __func__) 
		<< "The GenericObj Is Not Implemented." << enderr;
	return false;
}

bool
AosGenericObj::feedByJql(
	AosRundata *rdata,
	const OmnString &obj_name,
	const OmnString &json)
{
	AosSetErrorUser(rdata, __func__)
		<< "The GenericObj Is Not Implemented." << enderr;
	return false;
}

