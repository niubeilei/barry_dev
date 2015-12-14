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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DLLDataProcObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDLLDataProcObj::AosDLLDataProcObj(
		const OmnString &name,
		const OmnString &libname,
		const OmnString &method, 
		const OmnString &version)
:
AosDLLObj(eDataProc, libname, method, version),
mProcName(name)
{
}


AosDLLDataProcObj::~AosDLLDataProcObj()
{
}


AosDataProcObjPtr 
AosDLLDataProcObj::clone()
{
	OmnShouldNeverComeHere;
	return 0;
}


AosDataProcObjPtr 
AosDLLDataProcObj::createDataProc(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


AosDataProcObjPtr 
AosDLLDataProcObj::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}
