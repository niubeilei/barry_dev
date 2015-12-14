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
//
// Modification History:
// 03/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CommandProc/CommandProc.h"

#include "Thread/Mutex.h"
#include "SEServer/SeReqProc.h"
#include "Util/HashUtil.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Rundata.h"

AosCommandProc::AosCommandProc()
{
}


AosCommandProc::AosCommandProc(
		const AosRundataPtr &rdata,
		const u32 version, 
		const OmnString &name)
:
mJimoName(name)
{
}


OmnString 
AosCommandProc::toString() const
{
	OmnNotImplementedYet;
	return "";
}


bool 
AosCommandProc::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCommandProc::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCommandProc::AosCommandProc::config(
		const AosXmlTagPtr &def,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	return true;
}

