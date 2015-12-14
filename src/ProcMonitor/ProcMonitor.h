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
// 09/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ProcMonitor_ProcMonitor_h
#define Aos_ProcMonitor_ProcMonitor_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"



class AosProcMonitor : public AosNetReqProc
{
	OmnDefineRCObject;

private:

public:
	AosProcMonitor();
	~AosProcMonitor();

	static bool		config(const AosXmlTagPtr &config);
	static bool		stop();
	virtual bool	procRequest(const OmnConnBuffPtr &req);
	virtual AosNetReqProcPtr	clone();

	void 	sendResp(const AosWebRequestPtr &req, 
				const AosXmlRc errcode,
				const OmnString &errmsg, 
				const OmnString &contents); 
};
#endif

