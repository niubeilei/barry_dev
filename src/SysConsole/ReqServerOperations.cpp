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
// 2013/03/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SeReqProc/ServerOperations.h"

#include "SysConsole/SysConsole.h"
#include "SysConsole/ConsoleProc.h"
#include "XmlUtil/XmlTag.h"


AosServerOperations::AosServerOperations(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SERVER_OPERATIONS, AosSeReqid::eServerOperations, rflag)
{
}


bool 
AosServerOperations::proc(const AosRundataPtr &rdata)
{
	// This request retrieves server info. Its format is:
	// 	<req
	// 		operation="turnon|turnoff|reboot"
	//		cluster_id="xxx"
	//		servers="xxx,xxx,xxx"
	//		schedule="xxx"
	//		notification="xxx"/>
	
	AosXmlTagPtr root = rdata->getRequestRoot();

	if (!root)
	{
		AosSetErrorU(rdata, "missing_root");
		return false;
	}

	OmnString cluster_id = root->getAttrStr("cluster_id");
	if (cluster_id == "")
	{
		AosSetErrorU(rdata, "missing_cluster_id");
		return false;
	}

	AosClusterPtr cluster = AosGetSysConsole()->getCluster(rdata, cluster_id);
	if (!cluster)
	{
		AosSetErrorU(rdata, "clstuer_not_found") << ": " << cluster_id;
		return false;
	}

	OmnString str = root->getAttrStr("servers", "all");
	vector<OmnString> server_names;
	vector<int> server_ids;
	bool rslt = cluster->getServerAndIDs(rdata, str, server_names, server_ids);

	if (server_names.size() <= 0)
	{
		AosSetErrorU(rdata, "server_not_found") << ": " << str;
		return false;
	}

	if (server_names.size() != server_ids.size())
	{
		AosSetErrorU(rdata, "internal_error");
		return false;
	}

	OmnString operation = root->getAttrStr("operation");
	if (!isValidOperation(operation))
	{
		AosSetErrorU(rdata, "invalid_operation") << ": " << operation;
		return false;
	}

	// TBD
	// OmnString schedulestr = root->getAttrStr("schedule");
	// AosSchedule schedule = AosSchedule::convertSchedule(rdata, schedulestr);
	// if (!schedule.isValid())
	// {
	// 	AosSetErrorU(rdata, "invalid_schedule") << ": " << schedulestr;
	// 	return false;
	// }

	// TBD
	// bool rslt = AosCheckSecurity(rdata);
	// if (!rslt) return false;

	// When it comes to this point, we obtained the following:
	// 	1. The cluster
	// 	2. The servers
	// 	3. The operation
	// 	4. The schedule. 
	for (u32 i=0; i<server_names.size(); i++)
	{
		AosBuffMsgPtr msg = OmnNew AosMsgServerOperation(
				rdata, AosGetSelfPhysicalId(), server_ids[i], 
				server_names[i], operation, schedulestr);
		bool rslt = AosSendCubeMsg(rdata, server_ids[i], msg);
		if (!rslt)
		{
			AosSetErrorU(rdata, "failed_sending_msg") << ": " << rdata->getErrmsg();
			return false;
		}
	}

	return true;
}
#endif
