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
// 2013/03/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SeReqProc/GetServerInfo.h"

#include "SysConsole/SysConsole.h"
#include "SysConsole/ConsoleProc.h"
#include "XmlUtil/XmlTag.h"


AosGetServerInfo::AosGetServerInfo(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_GET_SERVER_INFO, AosSeReqid::eGetServerInfo, rflag)
{
}


bool 
AosGetServerInfo::proc(const AosRundataPtr &rdata)
{
	// This request retrieves server info. Its format is:
	// 	<req ...>
	// 		<entries>
	// 			<entry info_id="xxx" .../>
	// 			<entry info_id="xxx" .../>
	// 			...
	// 		</entries>
	// 	</req>
	// If <entries> is not there or it is empty, it is an error. 
	// Otherwise, it retrieves the info and returns the info
	// in the following form:
	// 	<Contents>
	// 		<entry info_id="xxx">
	// 			...
	// 		</entry>
	// 		<entry info_id="xxx">
	// 			...
	// 		</entry>
	// 		...
	// 	</Contents>
	
	AosXmlTagPtr root = rdata->getRequestRoot();

	if (!root)
	{
		AosSetErrorU(rdata, "missing_root");
		return false;
	}

	AosXmlTagPtr entries = root->getFirstChild("entries");
	if (!entries)
	{
		AosSetErrorU(rdata, "missing_entries");
		return false;
	}

	AosXmlTagPtr entry = entries->getFirstChild();
	if (!entry)
	{
		AosSetErrorU(rdata, "entries_empty");
		return false;
	}

	int guard = smMaxEntries;
	AosSeReqProcPtr request = rdata->getReqProc();
	if (!request)
	{
		AosSetErrorU(rdata, "internal_error");
		return false;
	}

	while (guard-- && entry)
	{
		OmnString info_id = entry->getAttrStr("info_id");
		if (info_id == "")
		{
			AosSetErrorU(rdata, "info_id_empty");
			return false;
		}

		AosConsoleProcPtr proc = AosConsoleProc::getProc(info_id);
		if (!proc)
		{
			AosSetErrorU(rdata, "invalid_info_id") << ": " << info_id << enderr;
			return false;
		}

		bool finished;
		OmnString resp;
		proc->procReq(rdata, entry, resp, finished);
		request->addResults(rdata, info_id, resp, finished);

		entry = entries->getNextChild();
	}

	return true;
}
#endif
