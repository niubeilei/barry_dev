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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CloudServer/CloudSvrMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/ExitHandler.h"
#include "XmlUtil/XmlTag.h"



AosCloudSvrMgr::AosCloudSvrMgr()
{
}


AosCloudSvrMgr::~AosCloudSvrMgr()
{
}


bool 
AosCloudSvrMgr::config(const AosXmlTagPtr &conf)
{
	// 	<config ...>
	// 		<AOSCONFIG_NETWORKMGR zky_numvirtuals="xxx">
	// 			...
	// 		</AOSCONFIG_NETWORKMGR>
	// 		...
	// 	</config>
	if (!conf)
	{
		OmnExitApp("Missing configuration");
		return false;
	}

	AosXmlTagPtr tag = conf->getFirstChild(AOSCONFIG_NETWORKMGR);
	if (!tag)
	{
		OmnExitApp("Missing NetworkMgr config");
		return false;
	}

	mNumVirtuals = tag->getAttrInt(AOSTAG_NUM_VIRTUALS, -1);
	if (mNumVirtuals <= 0 || mNumVirtuals > eMaxVirtuals)
	{
		OmnExitApp("Invalid number of virtuals");
		return false;
	}

	return true;
}


bool 
AosCloudSvrMgr::start()
{
	return true;
}


bool 
AosCloudSvrMgr::stop()
{
	return true;
}

