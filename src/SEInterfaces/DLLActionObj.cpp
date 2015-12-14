////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2013/05/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEInterfaces/DLLActionObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

// All dynamic actions must derive from this class. This class must not be
// an abstract class. The following statement creates an instance of 
// this class. If it is an abstract class, compiling will fail.
static AosDLLActionObj sgTestOnly("Test", "test", "test", "test");

AosDLLActionObj::AosDLLActionObj(
		const OmnString &name, 
		const OmnString &libname, 
		const OmnString &method,
		const OmnString &version)
:
AosDLLObj(eAction, libname, method, version),
mActionName(name)
{
}


AosDLLActionObj::~AosDLLActionObj()
{
}


bool 
AosDLLActionObj::run(const AosXmlTagPtr &def, const AosRundataPtr &rundata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDLLActionObj::run(const char *data, const int len, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDLLActionObj::run(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


AosActionObjPtr 
AosDLLActionObj::clone(const AosXmlTagPtr &def, const AosRundataPtr &) const
{
	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosDLLActionObj::run(const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rundata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDLLActionObj::checkConfig(
		const AosXmlTagPtr &def, 
		const AosTaskObjPtr &task, 
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}
	
bool
AosDLLActionObj::checkConfig(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf,
		set<OmnString> &data_col_ids)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosDLLActionObj::initAction(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosDLLActionObj::run(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosDLLActionObj::finishedAction(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


AosJimoPtr
AosDLLActionObj::cloneJimo() const
{
	return OmnNew AosDLLActionObj();
}
#endif
