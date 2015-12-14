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
// 2013/05/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DLLCondObj.h"

// This class must be a concrete class. The statement below tests
// whether it can create an instance.
static AosDLLCondObj sgTest("test", "test", "", "");


AosDLLCondObj::AosDLLCondObj(
		const OmnString &name, 
		const OmnString &libname, 
		const OmnString &method,
		const OmnString &version)
:
AosDLLObj(eCondition, libname, method, version)
{
}


AosDLLCondObj::~AosDLLCondObj()
{
}


AosConditionObjPtr 
AosDLLCondObj::clone()
{
	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosDLLCondObj::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDLLCondObj::evalCond(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDLLCondObj::evalCond(const char *record, const int len, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDLLCondObj::evalCond(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDLLCondObj::evalCond(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


AosConditionObjPtr 
AosDLLCondObj::getCondition(const AosXmlTagPtr &conf, const AosRundataPtr &)
{
	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosDLLCondObj::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

