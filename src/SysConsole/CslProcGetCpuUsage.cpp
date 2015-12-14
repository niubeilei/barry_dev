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
// 2013/03/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SysConsole/CslProcGetCpuUsage.h"

#include "SysConsole/InfoIds.h"

AosCslProcGetCpuUsage::AosCslProcGetCpuUsage(const bool flag)
:
AosConsoleProc(AOSCONSOLE_INFOID_GET_CPU_INFO, flag)
{
}


AosCslProcGetCpuUsage::~AosCslProcGetCpuUsage()
{
}


bool 
AosCslProcGetCpuUsage::procMsg(
		const AosRundataPtr &rdata,
		const AosXmLTagPtr &entry,
		OmnString &resp, 
		bool &finished)
{
	// A client wants to retrieve server info. 'entry' should
	// be in the following format:
	// 	<entry info_id="server_info"
	// 		cluster_id="xxx"
	// 		servers="xxx"
	// 		resource_id="xxx"
	// 		logdata="true|false"
	// 		starttime="xxx"
	// 		endtime="xxx"
	// 		resolution="xxx"/>
	resp = "";
	finished = true;
	OmnString cluster_id = entry->getAttrStr("cluster_id");
	if (cluster_id == "")
	{
		AosSetErrorU(rdata, "missing_cluster_id");
		return false;
	}

	OmnString servers = entry->getAttrStr("servers", "all");
}
#endif
